// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "F1ExCalcDamage.generated.h"

/**
 * 
 */
UCLASS()
class F1_API UF1ExCalcDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UF1ExCalcDamage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
