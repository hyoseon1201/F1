// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "F1PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class F1_API AF1PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AF1PlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputMappingContext> F1Context;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;

	void Move(const struct FInputActionValue& InputActionValue);
};
