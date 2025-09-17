// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "F1AssetManager.generated.h"

/**
 * 
 */
UCLASS()
class F1_API UF1AssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	static UF1AssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
