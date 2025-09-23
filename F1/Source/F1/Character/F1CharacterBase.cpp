#include "Character/F1CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"

AF1CharacterBase::AF1CharacterBase()
{
    PrimaryActorTick.bCanEverTick = false;

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // ±âº» ÆÀ ¼³Á¤
    TeamID = FGenericTeamId(0);
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

    // ÆÀº° Stencil Value ¼³Á¤ (1=Enemy Red, 2=Ally Blue)
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
