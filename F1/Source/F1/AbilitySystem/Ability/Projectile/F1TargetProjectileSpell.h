// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/Projectile/F1ProjectileSpell.h"
#include "F1TargetProjectileSpell.generated.h"

UCLASS()
class F1_API UF1TargetProjectileSpell : public UF1ProjectileSpell
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Target Projectile")
	void HandleRangeAndCast(const FVector& TargetLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Target Projectile")
	void K2_ExecuteCast(const FVector& TargetLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Target Projectile")
	void K2_MoveToRangeAndCast(const FVector& MoveLocation, const FVector& CastLocation);

private:
	bool IsWithinRange(const FVector& TargetLocation) const;
	FVector GetRangeLocation(const FVector& TargetLocation) const;
	void ExecuteCast(const FVector& TargetLocation);
	void MoveToRangeAndCast(const FVector& TargetLocation);
};
