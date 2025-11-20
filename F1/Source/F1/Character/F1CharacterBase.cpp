#include "Character/F1CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/F1UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "F1.h"

AF1CharacterBase::AF1CharacterBase()
{
    PrimaryActorTick.bCanEverTick = false;

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    GetCapsuleComponent()->SetGenerateOverlapEvents(false);

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);

    // 기본 팀 설정
    TeamID = FGenericTeamId(0);

    HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
    HealthBar->SetupAttachment(GetRootComponent());
}

void AF1CharacterBase::BeginPlay()
{
    Super::BeginPlay();
}


void AF1CharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AF1CharacterBase, TeamID);
}

void AF1CharacterBase::InitAbilityActorInfo()
{
}

void AF1CharacterBase::HighlightActor()
{
    if (bHighlighted) return;

    bHighlighted = true;

    GetMesh()->SetRenderCustomDepth(true);

    uint8 StencilValue = IsEnemyToPlayer() ? 1 : 2;
    GetMesh()->SetCustomDepthStencilValue(StencilValue);
}

void AF1CharacterBase::UnHighlightActor()
{
    if (!bHighlighted) return;

    bHighlighted = false;

    GetMesh()->SetRenderCustomDepth(false);
    GetMesh()->SetCustomDepthStencilValue(0);
}

bool AF1CharacterBase::IsEnemyToPlayer() const
{
    if (const UWorld* World = GetWorld())
    {
        if (APlayerController* PC = World->GetFirstPlayerController())
        {
            if (APawn* PlayerPawn = PC->GetPawn())
            {
                if (const IGenericTeamAgentInterface* PlayerTeamAgent = Cast<IGenericTeamAgentInterface>(PlayerPawn))
                {
                    FGenericTeamId PlayerTeam = PlayerTeamAgent->GetGenericTeamId();
                    FGenericTeamId MyTeam = GetGenericTeamId();
                    return PlayerTeam != MyTeam;
                }
            }
        }
    }

    return true;
}

int32 AF1CharacterBase::GetCurrentLevel() const
{
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        if (const UAttributeSet* AS = ASC->GetAttributeSet(UF1AttributeSet::StaticClass()))
        {
            const UF1AttributeSet* F1AS = Cast<UF1AttributeSet>(AS);
            return F1AS ? FMath::FloorToInt(F1AS->GetCharacterLevel()) : 1;
        }
    }
    return 1;
}

float AF1CharacterBase::GetCurrentExperience() const
{
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        if (const UAttributeSet* AS = ASC->GetAttributeSet(UF1AttributeSet::StaticClass()))
        {
            const UF1AttributeSet* F1AS = Cast<UF1AttributeSet>(AS);
            return F1AS ? F1AS->GetExperience() : 0.0f;
        }
    }
    return 0.0f;
}

void AF1CharacterBase::ApplyLevelBasedGrowth()
{
    UE_LOG(LogTemp, Log, TEXT("AF1CharacterBase::ApplyLevelBasedGrowth - Base implementation called"));
}

FVector AF1CharacterBase::GetCombatSocketLocation()
{
    USkeletalMeshComponent* CurrentMesh = GetMesh();
    check(CurrentMesh && TEXT("Mesh component is null in GetCombatSocketLocation"));

    const USkeletalMesh* SkeletalMeshAsset = CurrentMesh->GetSkeletalMeshAsset();
    check(SkeletalMeshAsset && TEXT("SkeletalMeshAsset is null in GetCombatSocketLocation"));

    TArray<FName> SocketPriority = {
        WeaponTipSocketName,
        MuzzleSocketName,
        HandSocketName,
        ChestSocketName
    };

    TArray<FName> AllSocketNames = CurrentMesh->GetAllSocketNames();

    if (AllSocketNames.Num() > 0)
    {
        TArray<FString> SocketStrings;
        for (const FName& SocketName : AllSocketNames)
        {
            SocketStrings.Add(SocketName.ToString());
        }
    }

    for (const FName& SocketName : SocketPriority)
    {
        if (!SocketName.IsValid())
        {
            continue;
        }

        if (SocketName == FName("None") || SocketName == NAME_None)
        {
            continue;
        }

        if (CurrentMesh->DoesSocketExist(SocketName))
        {
            FVector SocketLocation = CurrentMesh->GetSocketLocation(SocketName);
            return SocketLocation;
        }
    }

    FVector DefaultLocation = GetActorLocation() + GetActorForwardVector() * 50.0f;

    return DefaultLocation;
}

void AF1CharacterBase::Die()
{
    MulticastHandleDeath();
}

void AF1CharacterBase::MulticastHandleDeath_Implementation()
{
    GetMesh()->SetSimulatePhysics(true);
    GetMesh()->SetEnableGravity(true);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AF1CharacterBase::UpdateCombatSocketsFromCharacterInfo()
{

}

UAbilitySystemComponent* AF1CharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AF1CharacterBase::BindMovementSpeedDelegate()
{
    if (!AbilitySystemComponent || !AttributeSet || !GetCharacterMovement()) return;

    const UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);

    // 델리게이트 등록 및 초기값 동기화 처리
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMovementSpeedAttribute()).AddLambda(
        [this, F1AS](const FOnAttributeChangeData& Data)
        {
            // 델리게이트가 호출될 때마다 MaxWalkSpeed를 업데이트
            GetCharacterMovement()->MaxWalkSpeed = Data.NewValue;
        }
    );
}

void AF1CharacterBase::InitializeHealthBarWidget()
{
    // 중복 체크 및 필수 컴포넌트 체크 (유지)
    if (bHealthBarInitialized || !AbilitySystemComponent || !AttributeSet || !HealthBar) return;

    UUserWidget* Widget = HealthBar->GetUserWidgetObject();
    if (!Widget) return;

    UF1UserWidget* F1UserWidget = Cast<UF1UserWidget>(Widget);
    if (!F1UserWidget) return;

    // WidgetController 설정 (유지)
    F1UserWidget->SetWidgetController(this);

    UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);

    // 1. 델리게이트 바인딩 (유지)
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetHealthAttribute())
        .AddLambda([this](const FOnAttributeChangeData& Data) { OnHealthChanged.Broadcast(Data.NewValue); });

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMaxHealthAttribute())
        .AddLambda([this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); });


    // 2. 💡 수동으로 현재 값을 브로드캐스트합니다. (이 부분이 초기화 타이밍을 강제합니다)
    //    GAS 델리게이트가 자동으로 트리거되지 않거나, WBP의 숨김 로직을 잘못 트리거하는 것을 방지합니다.
    const float InitialHealth = F1AS->GetHealth();
    const float InitialMaxHealth = F1AS->GetMaxHealth();

    OnHealthChanged.Broadcast(InitialHealth);
    OnMaxHealthChanged.Broadcast(InitialMaxHealth);


    if (HealthBar)
    {
        HealthBar->SetHiddenInGame(false);
        HealthBar->SetVisibility(true);
    }
    bHealthBarInitialized = true;
}