// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "F1TargetDataValidTarget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FValidTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoValidTargetSignature);

/**
 *
 */
UCLASS()
class F1_API UF1TargetDataValidTarget : public UAbilityTask
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetDataValidTarget", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UF1TargetDataValidTarget* CreateTargetDataValidTarget(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FValidTargetDataSignature ValidTarget;

	UPROPERTY(BlueprintAssignable)
	FNoValidTargetSignature NoValidTarget;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Validation")
	bool bRequireEnemyTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Validation")
	bool bRequireAliveTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Target Validation")
	bool bRequireActorTarget = true;

private:

	virtual void Activate() override;
	void SendMouseCursorData();
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
	bool IsValidTarget(AActor* Target) const;
	bool IsEnemy(AActor* Target) const;
	bool IsAlive(AActor* Target) const;
};
