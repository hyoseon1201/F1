// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "F1BTTask_Attack.generated.h"

/**
 * 
 */
UCLASS()
class F1_API UF1BTTask_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UF1BTTask_Attack();

	// BT가 이 태스크를 실행할 때 호출되는 함수
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
