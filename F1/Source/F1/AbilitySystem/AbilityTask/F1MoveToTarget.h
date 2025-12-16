// F1MoveToTarget.h

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "F1MoveToTarget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMoveToTargetDelegate);

UCLASS()
class UF1MoveToTarget : public UAbilityTask
{
	GENERATED_BODY()

public:
	UF1MoveToTarget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE", AutoCreateRefTerm = "TargetLocation"))
	static UF1MoveToTarget* MoveToTarget(UGameplayAbility* OwningAbility, AActor* TargetActor, FVector TargetLocation, float StopDistance = 1500.f);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	UPROPERTY(BlueprintAssignable)
	FMoveToTargetDelegate OnReached;

	UPROPERTY(BlueprintAssignable)
	FMoveToTargetDelegate OnCancelled;

protected:
	UPROPERTY()
	TObjectPtr<AActor> TargetActor;

	FVector TargetLocation;
	float StopDistance;
	bool bIsFinished;

	void PerformFaceTarget();
	void CheckDistanceAndFinish();
};