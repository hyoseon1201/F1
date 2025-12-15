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

    // 1. GAS 연결 (모두에게 필수)
    InitAbilityActorInfo();

    // 2. 내 캐릭터일 때만 필요한 설정 (이동속도 등)
    if (IsLocallyControlled())
    {
        BindMovementSpeedDelegate();
        SyncMovementSpeedWithAttributeSet();
    }

    if (AF1PlayerState* F1PS = GetPlayerState<AF1PlayerState>())
    {
        SetGenericTeamId(F1PS->GetGenericTeamId());
    }

    InitUI();

    // 3. 메인 화면 HUD(스킬창, 경험치바 등)는 '나'만 있으면 됨
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        TWeakObjectPtr<AF1HeroCharacter> WeakThis(this);
        FTimerDelegate TimerDelegate;

        TimerDelegate.BindLambda([WeakThis, PC]()
            {
                if (!WeakThis.IsValid() || !IsValid(PC)) return;
                AF1HeroCharacter* StrongThis = WeakThis.Get();

                // 데이터 준비 확인
                bool bAttributesReady = (StrongThis->AttributeSet != nullptr);
                bool bAbilitiesReady = (StrongThis->AbilitySystemComponent != nullptr);

                if (bAttributesReady && bAbilitiesReady)
                {
                    if (UWorld* World = StrongThis->GetWorld())
                    {
                        World->GetTimerManager().ClearTimer(StrongThis->HUDInitTimerHandle);
                    }

                    // [여기서 InitUI를 또 부를 필요는 없습니다. 위에서 했으니까요.]

                    // 메인 화면 HUD 초기화 (이건 내 컨트롤러가 있을 때만!)
                    if (AF1HUD* HUD = Cast<AF1HUD>(PC->GetHUD()))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("[Client] HUD Initialized!"));
                        HUD->InitOverlay(PC, StrongThis->GetPlayerState(), StrongThis->AbilitySystemComponent, StrongThis->AttributeSet);
                    }
                }
            });

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

void AF1HeroCharacter::AddToLevel(int32 InLevelToAdd)
{
    AF1PlayerState* F1PS = GetPlayerState<AF1PlayerState>();

    if (F1PS)
    {
        const int32 CurrentLevel = F1PS->GetPlayerLevel();
        const int32 NewLevel = CurrentLevel + InLevelToAdd;

        F1PS->SetPlayerLevel(NewLevel);

        if (UF1AttributeSet* AS = Cast<UF1AttributeSet>(GetAttributeSet()))
        {
            AS->SetCharacterLevel(NewLevel);
        }
    }
}

void AF1HeroCharacter::LevelUp()
{
    ApplyLevelBasedGrowth();

    if (UF1AttributeSet* AS = Cast<UF1AttributeSet>(GetAttributeSet()))
    {
        AS->SetHealth(AS->GetMaxHealth());
        AS->SetMana(AS->GetMaxMana());
    }

    // TODO : LevelupEffect
}

void AF1HeroCharacter::ApplyLevelBasedGrowth()
{
    if (!HasAuthority() || !GrowthAttributes)
    {
        return;
    }

    int32 CurrentLevel = GetCurrentLevel();
    
    UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
    if (ASC)
    {
        FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
        ContextHandle.AddSourceObject(this);

        // [중요] 이펙트 레벨을 'CurrentLevel'로 설정
        // CurveTable에서 해당 레벨에 맞는 MaxHP, MaxMana 값을 가져오게 됨
        const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
            GrowthAttributes,
            static_cast<float>(CurrentLevel),
            ContextHandle
        );

        if (SpecHandle.Data.IsValid())
        {
            ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }
}

FVector AF1HeroCharacter::GetCombatSocketLocation()
{
    return Super::GetCombatSocketLocation();
}

void AF1HeroCharacter::MulticastHandleDeath_Implementation()
{
    // 1. 공통 기능 실행 (래그돌, 애니메이션 정지, GAS 취소)
    Super::MulticastHandleDeath_Implementation();

    // TODO : 2. 플레이어 입력차단 및 리스폰 타이머 시작 등등 로직 
}

void AF1HeroCharacter::Die()
{
    SetLifeSpan(LifeSpan); // TDOD: 부활로직 이후 삭제 예정
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

int32 AF1HeroCharacter::GetXPRequirement(int32 Level) const
{
    if (!LevelUpInfoTable) return 999999; // 안전장치

    // 테이블에서 "MaxXP"라는 이름의 행을 찾음
    const FRealCurve* LevelCurve = LevelUpInfoTable->FindCurve(FName("MaxXP"), FString());
    if (LevelCurve)
    {
        // 해당 레벨(X축)의 필요 경험치(Y축)를 반환
        return FMath::FloorToInt(LevelCurve->Eval(static_cast<float>(Level)));
    }
    return 999999;
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
    Super::InitAbilityActorInfo();

    AF1PlayerState* F1PS = GetPlayerState<AF1PlayerState>();
    if (!F1PS) return;

    // 1. GAS 연결 (Owner: PlayerState, Avatar: Character)
    F1PS->GetAbilitySystemComponent()->InitAbilityActorInfo(F1PS, this);
    Cast<UF1AbilitySystemComponent>(F1PS->GetAbilitySystemComponent())->AbilityActorInfoSet();

    // 2. 포인터 설정
    AbilitySystemComponent = F1PS->GetAbilitySystemComponent();
    AttributeSet = F1PS->GetAttributeSet();

    F1PS->SetPlayerLevel(F1PS->GetPlayerLevel());

    if (UF1AttributeSet* AS = Cast<UF1AttributeSet>(GetAttributeSet()))
    {
        // 시작 레벨(1)에 맞는 MaxXP(1000)를 강제로 세팅
        float InitMaxXP = static_cast<float>(GetXPRequirement(F1PS->GetPlayerLevel()));
        AS->SetMaxExperience(InitMaxXP);
    }
}