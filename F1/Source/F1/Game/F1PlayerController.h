// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "F1PlayerController.generated.h"

class IF1TeamOutlineInterface;
class AF1CharacterBase;

/**
 * 
 */
UCLASS()
class F1_API AF1PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AF1PlayerController();

	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputMappingContext> F1Context;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;

	void Move(const struct FInputActionValue& InputActionValue);

#pragma region outlining
private:
	void CursorTrace();

	UPROPERTY()
	AF1CharacterBase* LastActor;

	UPROPERTY()
	AF1CharacterBase* ThisActor;
#pragma endregion
};
