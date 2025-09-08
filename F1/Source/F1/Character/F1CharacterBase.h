// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "F1CharacterBase.generated.h"

UCLASS(Abstract)
class F1_API AF1CharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AF1CharacterBase();

protected:
	virtual void BeginPlay() override;
};
