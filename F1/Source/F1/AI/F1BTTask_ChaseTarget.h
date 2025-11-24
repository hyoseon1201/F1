// F1BTTask_ChaseTarget.h

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "F1BTTask_ChaseTarget.generated.h"

/**
 * 블랙보드의 AttackRange만큼 거리를 두고 타겟을 추격하는 태스크
 */
UCLASS()
class F1_API UF1BTTask_ChaseTarget : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UF1BTTask_ChaseTarget();

	// 태스크 실행 시 호출되는 함수
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	// 매 프레임 실행
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};