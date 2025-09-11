#include "Character/F1CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

AF1CharacterBase::AF1CharacterBase()
{
    PrimaryActorTick.bCanEverTick = false;

    // CapsuleComponent: 물리 충돌 + 마우스 감지 둘 다 유지
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // SkeletalMesh: 물리 충돌 비활성화, 마우스 감지만
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // 기본 팀 설정
    TeamID = FGenericTeamId(1);
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

#pragma region Outline Interface Implementation

void AF1CharacterBase::HighlightActor()
{
    if (bHighlighted) return;  // 이미 하이라이트된 상태면 무시

    bHighlighted = true;

    // Custom Depth 활성화 (Post Process Material 방식)
    GetMesh()->SetRenderCustomDepth(true);

    // 팀별 Stencil Value 설정 (1=Enemy Red, 2=Ally Blue)
    uint8 StencilValue = IsEnemyToPlayer() ? 1 : 2;
    GetMesh()->SetCustomDepthStencilValue(StencilValue);
}

void AF1CharacterBase::UnHighlightActor()
{
    if (!bHighlighted) return;  // 이미 비활성화된 상태면 무시

    bHighlighted = false;

    // Custom Depth 비활성화
    GetMesh()->SetRenderCustomDepth(false);
    GetMesh()->SetCustomDepthStencilValue(0);
}

bool AF1CharacterBase::IsEnemyToPlayer() const
{
    // 현재 플레이어와 팀 비교
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
                    return PlayerTeam != MyTeam;  // 다른 팀이면 적
                }
            }
        }
    }

    // 플레이어를 찾을 수 없으면 기본적으로 적으로 처리
    return true;
}

#pragma endregion
