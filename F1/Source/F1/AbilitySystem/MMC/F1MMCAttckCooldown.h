// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "F1MMCAttckCooldown.generated.h"

/**
 * 
 */
UCLASS()
class F1_API UF1MMCAttckCooldown : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UF1MMCAttckCooldown();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition AttackSpeedDef;
};
