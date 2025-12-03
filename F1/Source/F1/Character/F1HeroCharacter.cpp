// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/F1HeroCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Game/F1PlayerState.h"
#include "AbilitySystemComponent.h"
#include "Game/F1PlayerController.h"
#include "UI/HUD/F1HUD.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/F1AbilitySystemLibrary.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/F1UserWidget.h"

AF1HeroCharacter::AF1HeroCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

    PlayerBar = CreateDefaultSubobject<UWidgetComponent>("PlayerBar");
    PlayerBar->SetupAttachment(GetRootComponent());
}

void AF1HeroCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(AF1HeroCharacter, CurrentCharacterInfo, COND_None, REPNOTIFY_Always);
}

void AF1HeroCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // 서버 권한이 있을 때만 실행
    if (HasAuthority())
    {
        // 1. 데이터 설정
        SetCharacterClass(FName("Dekker"));

        // 2. GAS 연결
        InitAbilityActorInfo();

        // 3. 기타 델리게이트 설정
        SyncMovementSpeedWithAttributeSet();
        BindMovementSpeedDelegate();

        // 4. 스탯 초기화
        UF1AbilitySystemLibrary::InitializeDefaultAttributes(this);

        // 5. [중요] 스킬 배우기 (가방 채우기)
        // 이 함수가 실행되어야 ASC에 스킬이 들어갑니다.
        UF1AbilitySystemLibrary::AddCharacterAbilities(this);

        // 6. 레벨 성장 적용
        ApplyLevelBasedGrowth();

        // 7. 팀 ID 동기화
        if (AF1PlayerState* F1PS = GetPlayerState<AF1PlayerState>())
        {
            SetGenericTeamId(F1PS->GetGenericTeamId());
        }
    }

    // [핵심 추가] 8. HUD(오버레이) 초기화
    // 스킬을 다 배운 뒤에 HUD를 초기화해야 아이콘이 뜹니다.
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (AF1HUD* HUD = Cast<AF1HUD>(PC->GetHUD()))
        {
            HUD->InitOverlay(PC, GetPlayerState(), AbilitySystemComponent, AttributeSet);
        }
    }

    // 9. 캐릭터 머리 위 위젯 초기화
    InitUI();
}

void AF1HeroCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    InitAbilityActorInfo();

    if (IsLocallyControlled())
    {
        BindMovementSpeedDelegate();
        SyncMovementSpeedWithAttributeSet();
    }

    if (AF1PlayerState* F1PS = GetPlayerState<AF1PlayerState>())
    {
        SetGenericTeamId(F1PS->GetGenericTeamId());
    }

    // [핵심 수정] "데이터가 올 때까지 반복 체크" 하는 타이머
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        // 람다 내부에서 타이머를 제어하기 위해 TimerDelegate 사용
        FTimerDelegate TimerDelegate;

        TimerDelegate.BindLambda([this, PC]()
            {
                // 1. 유효성 검사 (캐릭터나 PC가 죽었으면 중단)
                if (!IsValid(this) || !IsValid(PC))
                {
                    GetWorld()->GetTimerManager().ClearTimer(HUDInitTimerHandle);
                    return;
                }

                // 2. 데이터 도착 확인 (ASC, AttributeSet, 그리고 *스킬 목록*)
                // ActivatableAbilities.Num() > 0 : 스킬이 최소 1개 이상 도착했다는 뜻
                bool bAttributesReady = (AttributeSet != nullptr);
                bool bAbilitiesReady = (AbilitySystemComponent != nullptr && AbilitySystemComponent->GetActivatableAbilities().Num() > 0);

                if (bAttributesReady && bAbilitiesReady)
                {
                    // 3. 데이터가 준비됨! -> 타이머 해제하고 초기화 진행
                    GetWorld()->GetTimerManager().ClearTimer(HUDInitTimerHandle);

                    if (AF1HUD* HUD = Cast<AF1HUD>(PC->GetHUD()))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("[Client] All Data Received! Initializing HUD..."));
                        HUD->InitOverlay(PC, GetPlayerState(), AbilitySystemComponent, AttributeSet);
                        InitUI();
                    }
                }
                else
                {
                    // 아직 안 옴... 다음 틱에 다시 확인 (로그는 너무 많이 뜨니 생략 가능)
                    // UE_LOG(LogTemp, Display, TEXT("[Client] Waiting for Abilities..."));
                }
            });

        // 0.1초 간격으로 반복(Loop) 실행
        GetWorld()->GetTimerManager().SetTimer(HUDInitTimerHandle, TimerDelegate, 0.1f, true);
    }
}

void AF1HeroCharacter::SetCharacterClass(FName CharacterRowName)
{
    // ... 유효성 검사 (유지) ...
    if (!HasAuthority() || !CharacterClassDataTable) return;

    FCharacterClassInfo* ClassInfo = CharacterClassDataTable->FindRow<FCharacterClassInfo>(CharacterRowName, TEXT("SetCharacterClass"));
    if (!ClassInfo) return;

    CurrentCharacterInfo = *ClassInfo;
    DefaultAttributes = CurrentCharacterInfo.DefaultAttributes;
    GrowthAttributes = CurrentCharacterInfo.GrowthAttributes;
    StartupAbilities = CurrentCharacterInfo.StartupAbilities;

    UpdateCombatSocketsFromCharacterInfo();

    ApplyVisualsFromCurrentInfo();
}

int32 AF1HeroCharacter::GetCurrentLevel() const
{
    int32 BaseLevel = Super::GetCurrentLevel();

    return BaseLevel;
}

float AF1HeroCharacter::GetCurrentExperience() const
{
    return Super::GetCurrentExperience();
}

void AF1HeroCharacter::ApplyLevelBasedGrowth()
{
    if (!HasAuthority() || !GrowthAttributes)
    {
        return;
    }

    int32 CurrentLevel = GetCurrentLevel();
    if (CurrentLevel <= 1)
    {
        return;
    }

    const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
    const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(
        GrowthAttributes,
        CurrentLevel - 1,
        ContextHandle
    );
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

FVector AF1HeroCharacter::GetCombatSocketLocation()
{
    return Super::GetCombatSocketLocation();
}

void AF1HeroCharacter::Die()
{
    SetLifeSpan(LifeSpan);
    Super::Die();
}

void AF1HeroCharacter::InitUI()
{
    // 1. 필수 데이터와 위젯 컴포넌트가 있는지 확인 (방어 코드)
    APlayerState* PS = GetPlayerState();
    if (!PS || !AbilitySystemComponent || !AttributeSet) return;
    if (!PlayerBar) return;

    UUserWidget* Widget = PlayerBar->GetUserWidgetObject();

    // [추가] 만약 없으면 강제로 만드세요.
    if (!Widget)
    {
        PlayerBar->InitWidget(); // 강제 초기화
        Widget = PlayerBar->GetUserWidgetObject();
    }

    // 2. 위젯 컴포넌트에서 실제 위젯 객체(UserWidget)를 가져옴
    UF1UserWidget* HeroBarWidget = Cast<UF1UserWidget>(Widget);

    // 3. 컨트롤러 클래스가 설정되어 있는지 확인
    if (HeroBarWidget && HeroWidgetControllerClass)
    {
        // 4. 파라미터 구조체 생성 (핵심 데이터 4인방 수집)
        const FWidgetControllerParams Params(
            GetController<APlayerController>(),
            PS,
            AbilitySystemComponent,
            AttributeSet
        );

        // 5. 위젯 컨트롤러 객체 생성 (NewObject)
        // 여기서 BP_HeroWidgetController가 생성됩니다.
        UF1WidgetController* WidgetController = NewObject<UF1WidgetController>(this, HeroWidgetControllerClass);

        // 6. 컨트롤러에 데이터 주입
        WidgetController->SetWidgetControllerParams(Params);

        // 7. 위젯에게 컨트롤러 전달 (이때 BP의 'Event WidgetControllerSet'이 발동!)
        HeroBarWidget->SetWidgetController(WidgetController);

        // 8. 초기값 방송 및 델리게이트 바인딩 (화면 갱신 시작)
        WidgetController->BroadcastInitialValues();
        WidgetController->BindCallbacksToDependencies();
    }
}

void AF1HeroCharacter::OnRep_CurrentCharacterInfo()
{
    ApplyVisualsFromCurrentInfo();
}

void AF1HeroCharacter::ApplyVisualsFromCurrentInfo()
{
    if (CurrentCharacterInfo.CharacterMesh)
    {
        GetMesh()->SetSkeletalMesh(CurrentCharacterInfo.CharacterMesh);
    }

    if (CurrentCharacterInfo.AnimBlueprint)
    {
        GetMesh()->SetAnimInstanceClass(CurrentCharacterInfo.AnimBlueprint);
    }

    GetCapsuleComponent()->SetCapsuleRadius(CurrentCharacterInfo.CapsuleRadius);
    GetCapsuleComponent()->SetCapsuleHalfHeight(CurrentCharacterInfo.CapsuleHalfHeight);

    GetMesh()->SetRelativeLocation(CurrentCharacterInfo.MeshRelativeLocation);
    GetMesh()->SetRelativeRotation(CurrentCharacterInfo.MeshRelativeRotation);
}

void AF1HeroCharacter::SyncMovementSpeedWithAttributeSet()
{
    if (const UF1AttributeSet* AS = Cast<UF1AttributeSet>(AttributeSet))
    {
        if (GetCharacterMovement())
        {
            float CurrentSpeed = AS->GetMovementSpeed();
            GetCharacterMovement()->MaxWalkSpeed = CurrentSpeed;;
        }
    }
}

void AF1HeroCharacter::UpdateCombatSocketsFromCharacterInfo()
{
    WeaponTipSocketName = CurrentCharacterInfo.WeaponTipSocketName;
    MuzzleSocketName = CurrentCharacterInfo.MuzzleSocketName;
    HandSocketName = CurrentCharacterInfo.HandSocketName;
    ChestSocketName = CurrentCharacterInfo.ChestSocketName;
}

void AF1HeroCharacter::InitAbilityActorInfo()
{
    AF1PlayerState* F1PlayerState = GetPlayerState<AF1PlayerState>();
    if (!F1PlayerState) return;

    // 1. GAS 연결 (Owner: PlayerState, Avatar: Character)
    F1PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(F1PlayerState, this);
    Cast<UF1AbilitySystemComponent>(F1PlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

    // 2. 포인터 설정
    AbilitySystemComponent = F1PlayerState->GetAbilitySystemComponent();
    AttributeSet = F1PlayerState->GetAttributeSet();
}