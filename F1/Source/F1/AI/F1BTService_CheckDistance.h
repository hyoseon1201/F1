// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "F1BTService_CheckDistance.generated.h"

/**
 * 
 */
UCLASS()
class F1_API UF1BTService_CheckDistance : public UBTService
{
	GENERATED_BODY()
	
public:
	UF1BTService_CheckDistance();

protected:
	// 매 틱(Interval)마다 실행되는 핵심 함수
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	// 에디터에서 블랙보드 키를 지정할 수 있게 함 (하드코딩 방지)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HomeLocationKey;   // 원래 위치 키

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;    // 타겟 액터 키

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsReturningKey;    // 복귀 중인지 여부 키

	// 거리 설정값
	UPROPERTY(EditAnywhere, Category = "Config")
	float MaxLeashRange = 2000.0f;    // 이 거리보다 멀어지면 복귀 시작

	UPROPERTY(EditAnywhere, Category = "Config")
	float ReturnAcceptanceRadius = 100.0f; // 이 거리 안으로 들어오면 복귀 완료
};
