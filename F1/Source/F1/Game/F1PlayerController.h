// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "F1PlayerController.generated.h"

class IF1TeamOutlineInterface;
class AF1CharacterBase;
class UF1InputConfig;
class UF1AbilitySystemComponent;

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

	// ===========================================
	// Outlining
	// ===========================================
private:
	void CursorTrace();

	UPROPERTY()
	AF1CharacterBase* LastActor;

	UPROPERTY()
	AF1CharacterBase* ThisActor;

	float CursorTraceInterval = 0.1f;
	float LastCursorTraceTime = 0.0f;

	// ===========================================
	// Input
	// ===========================================
private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UF1InputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UF1AbilitySystemComponent> F1AbilitySystemComponent;

	UF1AbilitySystemComponent* GetASC();

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
};
