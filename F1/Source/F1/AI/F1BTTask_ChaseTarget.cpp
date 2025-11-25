#include "AI/F1BTTask_ChaseTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"

UF1BTTask_ChaseTarget::UF1BTTask_ChaseTarget()
{
    NodeName = TEXT("Chase Target Dynamic");
    BlackboardKey.SelectedKeyName = FName("TargetActor");
    bNotifyTick = true;
}

EBTNodeResult::Type UF1BTTask_ChaseTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIC = OwnerComp.GetAIOwner();
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!AIC || !Blackboard) return EBTNodeResult::Failed;

    AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(BlackboardKey.SelectedKeyName));
    if (!TargetActor) return EBTNodeResult::Failed;

    // 거리 계산 및 보정
    float AttackRange = Blackboard->GetValueAsFloat(FName("AttackRange"));
    if (AttackRange <= 0.0f) AttackRange = 150.0f; // 안전장치

    // [중요] MoveToActor의 AcceptanceRadius는 "멈출 거리"입니다.
    // 캡슐 반경(약 30~40)을 고려해서 AttackRange보다 확실히 작게 잡아야 합니다.
    // 공격 사거리가 600이면, 500정도까지만 가도 충분합니다.
    float AcceptanceRadius = FMath::Max(0.0f, AttackRange - 150.0f);

    // [디버그] 시작 로그
    UE_LOG(LogTemp, Warning, TEXT("[Chase] Start! Range: %f, AcceptRadius: %f"), AttackRange, AcceptanceRadius);

    EPathFollowingRequestResult::Type MoveResult = AIC->MoveToActor(TargetActor, AcceptanceRadius, true, true, true, 0, true);

    if (MoveResult == EPathFollowingRequestResult::RequestSuccessful)
    {
        return EBTNodeResult::InProgress;
    }
    else if (MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Chase] Already At Goal!"));
        return EBTNodeResult::Succeeded;
    }

    UE_LOG(LogTemp, Error, TEXT("[Chase] Move Request Failed!"));
    return EBTNodeResult::Failed;
}

void UF1BTTask_ChaseTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIC = OwnerComp.GetAIOwner();
    APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Pawn || !Blackboard)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(BlackboardKey.SelectedKeyName));
    if (!TargetActor)
    {
        FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
        return;
    }

    // 거리 데이터 갱신
    float AttackRange = Blackboard->GetValueAsFloat(FName("AttackRange"));
    if (AttackRange <= 0.0f) AttackRange = 150.0f;

    // [주의] ExecuteTask와 똑같은 공식이어야 합니다.
    float AcceptanceRadius = FMath::Max(0.0f, AttackRange - 100.0f);

    float Distance = FVector::Dist(Pawn->GetActorLocation(), TargetActor->GetActorLocation());

    // [시각적 디버깅] 몬스터 머리 위에 거리 표시
    FString DebugMsg = FString::Printf(TEXT("Dist: %.1f / StopAt: %.1f"), Distance, AcceptanceRadius);
    DrawDebugString(GetWorld(), Pawn->GetActorLocation() + FVector(0, 0, 100), DebugMsg, nullptr, FColor::Yellow, 0.0f);

    // [핵심 수정] 거리 체크 조건 완화
    // 멈추려는 거리보다 조금 더(Tolerance) 여유를 둡니다.
    // MoveToActor는 정확한 지점에 멈추지 않을 수도 있습니다.
    float StopTolerance = 20.0f;

    if (Distance <= (AcceptanceRadius + StopTolerance))
    {
        UE_LOG(LogTemp, Warning, TEXT("[Chase] Reached! Dist: %f <= %f"), Distance, AcceptanceRadius + StopTolerance);

        AIC->StopMovement(); // 확실하게 멈춤
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
    else
    {
        // [중요] 플레이어가 도망가면 경로를 업데이트해야 함!
        // 매 프레임 MoveTo를 호출하면 비용이 비싸지만, 멀어졌을 때 재호출하는 로직이 필요할 수 있음.
        // 하지만 기본 MoveToActor는 타겟이 움직이면 알아서 경로를 재계산함.

        // 혹시 AI가 멈춰버렸는지 확인 (속도가 0인데 아직 목표 도달 못함?)
        if (Pawn->GetVelocity().SizeSquared() < 10.0f)
        {
            // 갇혔거나 길을 못 찾음 -> 일단 성공 처리해서 다시 판단하게 하거나, 재시도
            // 여기서는 로그만 찍어봅니다.
            // UE_LOG(LogTemp, Warning, TEXT("[Chase] Stuck? Velocity is 0"));
        }
    }
}