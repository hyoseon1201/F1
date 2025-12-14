// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/F1DamageGameplayAbility.h"
#include "F1MeleeSpell.generated.h"

/**
 * 
 */
UCLASS()
class F1_API UF1MeleeSpell : public UF1DamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Melee")
	void ApplyDamageToTarget(AActor* TargetActor);
};
