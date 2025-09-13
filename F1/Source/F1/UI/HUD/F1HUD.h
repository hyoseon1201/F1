// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "F1HUD.generated.h"

class UF1UserWidget;
class UF1OverlayWidgetController;
struct FWidgetControllerParams;

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

	UF1OverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UF1UserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UF1OverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UF1OverlayWidgetController> OverlayWidgetControllerClass;
};
