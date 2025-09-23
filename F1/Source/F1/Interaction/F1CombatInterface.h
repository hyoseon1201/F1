// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "F1CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UF1CombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class F1_API IF1CombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetCurrentLevel() const = 0;
	virtual float GetCurrentExperience() const = 0;
	virtual void ApplyLevelBasedGrowth() = 0;

	virtual FVector GetCombatSocketLocation() = 0;
};
