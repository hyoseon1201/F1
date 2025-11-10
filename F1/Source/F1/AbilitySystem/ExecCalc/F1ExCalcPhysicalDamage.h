// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "F1ExCalcPhysicalDamage.generated.h"

/**
 * 
 */
UCLASS()
class F1_API UF1ExCalcPhysicalDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UF1ExCalcPhysicalDamage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
