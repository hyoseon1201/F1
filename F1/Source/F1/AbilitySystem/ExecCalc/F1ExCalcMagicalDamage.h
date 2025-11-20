// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "F1ExCalcMagicalDamage.generated.h"

/**
 * 
 */
UCLASS()
class F1_API UF1ExCalcMagicalDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UF1ExCalcMagicalDamage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
