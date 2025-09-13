// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "F1HUD.generated.h"

class UF1UserWidget;

/**
 * 
 */
UCLASS()
class F1_API AF1HUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	TObjectPtr<UF1UserWidget> OverlayWidget;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UF1UserWidget> OverlayWidgetClass;
};
