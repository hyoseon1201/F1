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


void AF1CharacterBase::UpdateCombatSocketsFromCharacterInfo()
{

}

UAbilitySystemComponent* AF1CharacterBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AF1CharacterBase::InitializeDefaultAttributes()
{
    if (!HasAuthority()) return;
    check(IsValid(GetAbilitySystemComponent()));
    check(DefaultAttributes);

    const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
    const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultAttributes, 1.f, ContextHandle);
    GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AF1CharacterBase::AddCharacterAbilities()
{
    UF1AbilitySystemComponent* F1ASC = CastChecked<UF1AbilitySystemComponent>(AbilitySystemComponent);
    if (!HasAuthority()) return;
    F1ASC->AddCharacterAbilities(StartupAbilities);
}

void AF1CharacterBase::InitializeHealthBarWidget()
{
    UE_LOG(LogTemp, Warning, TEXT("========== InitializeHealthBarWidget =========="));
    UE_LOG(LogTemp, Warning, TEXT("Character: %s"), *GetName());

    // 1. 중복 초기화 방지
    if (bHealthBarInitialized)
    {
        UE_LOG(LogTemp, Warning, TEXT("HealthBar already initialized - Returning"));
        UE_LOG(LogTemp, Warning, TEXT("==============================================="));
        return;
    }

    // 2. ASC/AttributeSet 체크
    UE_LOG(LogTemp, Warning, TEXT("Checking ASC and AttributeSet..."));
    UE_LOG(LogTemp, Warning, TEXT("  - ASC: %s"), AbilitySystemComponent ? TEXT("Valid") : TEXT("NULL"));
    UE_LOG(LogTemp, Warning, TEXT("  - AttributeSet: %s"), AttributeSet ? TEXT("Valid") : TEXT("NULL"));

    if (!AbilitySystemComponent || !AttributeSet)
    {
        UE_LOG(LogTemp, Error, TEXT("ASC or AttributeSet is null - Cannot initialize"));
        UE_LOG(LogTemp, Warning, TEXT("==============================================="));
        return;
    }

    // 3. HealthBar Component 체크
    UE_LOG(LogTemp, Warning, TEXT("Checking HealthBar Component..."));
    UE_LOG(LogTemp, Warning, TEXT("  - HealthBar Component: %s"), HealthBar ? TEXT("Valid") : TEXT("NULL"));

    if (!HealthBar)
    {
        UE_LOG(LogTemp, Error, TEXT("HealthBar Component is NULL!"));
        UE_LOG(LogTemp, Warning, TEXT("==============================================="));
        return;
    }

    // 4. Widget Class 체크
    UClass* WidgetClass = HealthBar->GetWidgetClass();
    UE_LOG(LogTemp, Warning, TEXT("Checking Widget Class..."));
    UE_LOG(LogTemp, Warning, TEXT("  - Widget Class: %s"), WidgetClass ? *WidgetClass->GetName() : TEXT("NULL"));

    if (!WidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("Widget Class is NOT SET!"));
        UE_LOG(LogTemp, Error, TEXT("  - Check Blueprint: Components -> HealthBar -> Details -> Widget Class"));
        UE_LOG(LogTemp, Warning, TEXT("==============================================="));
        return;
    }

    // 5. UserWidgetObject 체크 (초기 상태)
    UUserWidget* ExistingWidget = HealthBar->GetUserWidgetObject();
    UE_LOG(LogTemp, Warning, TEXT("Initial UserWidgetObject state:"));
    UE_LOG(LogTemp, Warning, TEXT("  - UserWidgetObject: %s"), ExistingWidget ? TEXT("Already Created") : TEXT("NULL - Not Created Yet"));

    // 6. Widget 강제 초기화
    if (!ExistingWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("Widget not created yet, calling InitWidget()..."));
        HealthBar->InitWidget();

        // InitWidget 호출 후 다시 체크
        ExistingWidget = HealthBar->GetUserWidgetObject();
        UE_LOG(LogTemp, Warning, TEXT("After InitWidget():"));
        UE_LOG(LogTemp, Warning, TEXT("  - UserWidgetObject: %s"), ExistingWidget ? TEXT("Created Successfully") : TEXT("STILL NULL"));
    }

    // 7. Widget 획득 및 검증
    UUserWidget* Widget = HealthBar->GetUserWidgetObject();
    if (!Widget)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to get UserWidgetObject!"));
        UE_LOG(LogTemp, Error, TEXT("  - Widget Class is set: %s"), *WidgetClass->GetName());
        UE_LOG(LogTemp, Error, TEXT("  - But Widget creation failed"));
        UE_LOG(LogTemp, Error, TEXT("  - Possible causes:"));
        UE_LOG(LogTemp, Error, TEXT("    1. Widget BP has compilation errors"));
        UE_LOG(LogTemp, Error, TEXT("    2. WidgetComponent not fully initialized yet"));
        UE_LOG(LogTemp, Error, TEXT("    3. Timing issue - try delaying initialization"));
        UE_LOG(LogTemp, Warning, TEXT("==============================================="));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Widget obtained successfully: %s"), *Widget->GetName());
    UE_LOG(LogTemp, Warning, TEXT("  - Widget Class: %s"), *Widget->GetClass()->GetName());

    // 8. F1UserWidget 캐스팅
    UF1UserWidget* F1UserWidget = Cast<UF1UserWidget>(Widget);
    UE_LOG(LogTemp, Warning, TEXT("Casting to F1UserWidget..."));

    if (!F1UserWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("Widget is NOT F1UserWidget!"));
        UE_LOG(LogTemp, Error, TEXT("  - Expected: UF1UserWidget"));
        UE_LOG(LogTemp, Error, TEXT("  - Actual: %s"), *Widget->GetClass()->GetName());
        UE_LOG(LogTemp, Error, TEXT("  - Check if WBP_HealthBar parent class is F1UserWidget"));
        UE_LOG(LogTemp, Warning, TEXT("==============================================="));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Cast successful - Setting WidgetController"));

    // 9. WidgetController 설정
    F1UserWidget->SetWidgetController(this);
    UE_LOG(LogTemp, Warning, TEXT("WidgetController set successfully"));

    // 10. AttributeSet 델리게이트 바인딩
    UE_LOG(LogTemp, Warning, TEXT("Binding Attribute delegates..."));
    UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetHealthAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnHealthChanged.Broadcast(Data.NewValue);
        }
    );

    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMaxHealthAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            OnMaxHealthChanged.Broadcast(Data.NewValue);
        }
    );

    UE_LOG(LogTemp, Warning, TEXT("Delegates bound successfully"));

    // 11. 초기값 브로드캐스트
    float InitialHealth = F1AS->GetHealth();
    float InitialMaxHealth = F1AS->GetMaxHealth();

    UE_LOG(LogTemp, Warning, TEXT("Broadcasting initial values:"));
    UE_LOG(LogTemp, Warning, TEXT("  - Health: %.2f"), InitialHealth);
    UE_LOG(LogTemp, Warning, TEXT("  - MaxHealth: %.2f"), InitialMaxHealth);

    OnHealthChanged.Broadcast(InitialHealth);
    OnMaxHealthChanged.Broadcast(InitialMaxHealth);

    bHealthBarInitialized = true;
    UE_LOG(LogTemp, Warning, TEXT("HealthBar initialization complete!"));
    UE_LOG(LogTemp, Warning, TEXT("==============================================="));
}
