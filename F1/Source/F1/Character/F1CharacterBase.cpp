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
#include <Game/F1PlayerState.h>

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

    GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
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
    if (AttributeSet)
    {
        F1AttributeSet = Cast<UF1AttributeSet>(AttributeSet);
    }
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
    // 1. 월드나 PC가 없으면 적대적으로 간주 (안전장치)
    const UWorld* World = GetWorld();
    if (!World) return true;

    // 2. 로컬 플레이어 컨트롤러 가져오기
    // (GetFirstPlayerController는 클라이언트 입장에서 '나'를 의미하므로 UI용으로 적합)
    const APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return true;

    // 3. 플레이어 폰(캐릭터) 가져오기
    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn) return true;

    // ==========================================================
    // ⭐ [변경] CombatInterface를 통해 팀 ID 비교 ⭐
    // ==========================================================

    // 플레이어(상대방)가 전투 인터페이스를 가지고 있는지 확인
    const IF1CombatInterface* PlayerCombat = Cast<IF1CombatInterface>(PlayerPawn);

    // 나 자신(this)은 이미 인터페이스를 상속받았으므로 바로 호출 가능
    // (AF1CharacterBase가 IF1CombatInterface를 상속받았다는 가정)

    if (PlayerCombat)
    {
        // 내 팀 ID와 플레이어의 팀 ID가 다르면 적(Enemy)
        return GetTeamID() != PlayerCombat->GetTeamID();
    }

    return true; // 인터페이스가 없으면 기본적으로 적 취급
}

int32 AF1CharacterBase::GetCurrentLevel() const
{
    // 1. 플레이어라면 PlayerState에서 가져옴 (가장 우선순위)
    // (참고: 나중에 F1PlayerState도 캐싱해두면 여기서 Cast 비용도 줄일 수 있습니다)
    if (AF1PlayerState* F1PS = GetPlayerState<AF1PlayerState>())
    {
        return F1PS->GetPlayerLevel();
    }

    // 2. [최적화] 미니언/몬스터라면 캐싱된 AttributeSet에서 바로 가져옴
    if (F1AttributeSet)
    {
        // float -> int 변환
        return static_cast<int32>(F1AttributeSet->GetCharacterLevel());
    }

    // 3. 둘 다 없으면 기본 레벨 1
    return 1;
}

float AF1CharacterBase::GetCurrentExperience() const
{
    if (F1AttributeSet)
    {
        F1AttributeSet->GetExperience();
    }

    return 0.0f;
}

int32 AF1CharacterBase::GetTeamID() const
{
    return GetGenericTeamId();
}

void AF1CharacterBase::AddToLevel(int32 InLevelToAdd)
{
}

void AF1CharacterBase::LevelUp()
{
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

AActor* AF1CharacterBase::GetCombatTarget_Implementation() const
{
    return nullptr;
}

int32 AF1CharacterBase::GetRewardXP_Implementation()
{
    return int32();
}

int32 AF1CharacterBase::GetRewardGold_Implementation()
{
    return int32();
}

void AF1CharacterBase::Die()
{
    MulticastHandleDeath();
}

void AF1CharacterBase::SetCombatTarget(AActor* InTarget)
{
}

void AF1CharacterBase::Attack()
{
}

float AF1CharacterBase::GetAttackRange()
{
    if (F1AttributeSet)
    {
        return F1AttributeSet->GetAttackRange();
    }

    return 150.f;
}

void AF1CharacterBase::MulticastHandleDeath_Implementation()
{
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        ASC->CancelAllAbilities();
    }

    // [공통 2] 애니메이션 강제 중단
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
        {
            AnimInstance->StopAllMontages(0.0f);
        }
    }

    // [공통 3] 물리(Ragdoll) 처리
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
    // 안전장치
    if (!AbilitySystemComponent || !F1AttributeSet || !GetCharacterMovement()) return;

    // 1. 델리게이트 등록
    AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMovementSpeedAttribute()).AddLambda(
        [this](const FOnAttributeChangeData& Data)
        {
            if (UCharacterMovementComponent* CMC = GetCharacterMovement())
            {
                CMC->MaxWalkSpeed = Data.NewValue;
            }
        }
    );
}

void AF1CharacterBase::InitUI()
{
}