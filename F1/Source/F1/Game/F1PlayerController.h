// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "F1PlayerController.generated.h"

class IF1TeamOutlineInterface;
class AF1CharacterBase;
class UF1InputConfig;
class UF1AbilitySystemComponent;
class USplineComponent;

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

	UFUNCTION(BlueprintCallable, Category = "Team")
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor* Actor) const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartMovementToDestination();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputMappingContext> F1Context;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;

	void Move(const struct FInputActionValue& InputActionValue);
	void AutoRun();

	// ===========================================
	// Outlining
	// ===========================================
private:
	void CursorTrace();

	AF1CharacterBase* LastActor;
	AF1CharacterBase* ThisActor;
	FHitResult CursorHit;

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

	// ===========================================
	// Combat
	// ===========================================
private:
	bool IsEnemy(const AActor* Actor) const;
};
