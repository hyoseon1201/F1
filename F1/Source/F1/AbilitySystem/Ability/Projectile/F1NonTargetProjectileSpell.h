// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/Projectile/F1ProjectileSpell.h"
#include "F1NonTargetProjectileSpell.generated.h"

UCLASS()
class F1_API UF1NonTargetProjectileSpell : public UF1ProjectileSpell
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "NonTarget Projectile")
	void K2_ExecuteCast(const FVector& TargetLocation);

private:
	FVector GetMouseWorldDirection() const;
	FVector CalculateMaxRangeLocation() const;
	void ExecuteCast();
};
