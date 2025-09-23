// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/F1GameplayAbility.h"
#include "F1ProjectileSpell.generated.h"

class AF1Projectile;

/**
 * 
 */
UCLASS()
class F1_API UF1ProjectileSpell : public UF1GameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AF1Projectile> ProjectileClass;
};
