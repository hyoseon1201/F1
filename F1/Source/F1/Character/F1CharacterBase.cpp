#include "Character/F1CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"

AF1CharacterBase::AF1CharacterBase()
{
    PrimaryActorTick.bCanEverTick = false;

    // CapsuleComponent: ���� �浹 + ���콺 ���� �� �� ����
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // SkeletalMesh: ���� �浹 ��Ȱ��ȭ, ���콺 ������
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

    // �⺻ �� ����
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
    if (bHighlighted) return;

    bHighlighted = true;

    GetMesh()->SetRenderCustomDepth(true);

    // ���� Stencil Value ���� (1=Enemy Red, 2=Ally Blue)
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

#pragma endregion
