// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "F1MoveToTarget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveToTargetDelegate);

/**
 * 타겟 액터 혹은 특정 지점까지 이동하는 어빌리티 태스크
 * - TargetActor가 있으면 액터를 계속 추적하며 이동합니다.
 * - TargetActor가 없고 TargetLocation만 있으면 해당 지점으로 이동합니다.
 */
UCLASS()
class F1_API UF1MoveToTarget : public UAbilityTask
{
	GENERATED_BODY()

public:
	UF1MoveToTarget(const FObjectInitializer& ObjectInitializer);

	/**
	 * MoveToTarget Factory Function
	 * @param TargetActor (Optional) 추적할 액터. 연결하면 위치보다 우선순위를 가집니다.
	 * @param TargetLocation (Optional) 이동할 목표 지점. TargetActor가 없을 때 사용됩니다.
	 * @param StopDistance 멈출 거리 (사거리)
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE", AutoCreateRefTerm = "TargetLocation"))
	static UF1MoveToTarget* MoveToTarget(UGameplayAbility* OwningAbility, AActor* TargetActor, FVector TargetLocation, float StopDistance = 1500.f);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	UPROPERTY(BlueprintAssignable)
	FMoveToTargetDelegate OnReached;   // 도착 성공

	UPROPERTY(BlueprintAssignable)
	FMoveToTargetDelegate OnCancelled; // 실패/취소

protected:
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;

	FVector TargetLocation;
	float StopDistance;
	bool bIsFinished;
};