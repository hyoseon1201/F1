// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "F1PlayerController.generated.h"

class AF1CharacterBase;
class UF1InputConfig;
class UF1AbilitySystemComponent;
class USplineComponent;
class UDamageTextComponent;

UCLASS()
class F1_API AF1PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AF1PlayerController();

	UFUNCTION(BlueprintCallable)
	void StartAbilityMovementToDestination(const FVector& Destination);

	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StartMovementToDestination();

	UFUNCTION(BlueprintCallable)
	bool IsAutoRunning() const { return bAutoRunning; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputMappingContext> F1Context;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(Replicated)
	FVector CachedDestination = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> TargetEnemy = nullptr;

	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;

	UPROPERTY(Replicated)
	bool bAutoRunning = false;

	void AutoRun();
	void TraceAndAttackTarget();

	// ===========================================
	// Cursor Trace
	// ===========================================
private:
	void CursorTrace();

	AF1CharacterBase* LastActor = nullptr;
	AF1CharacterBase* ThisActor = nullptr;
	FHitResult CursorHit;

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
	// Damage
	// ===========================================
public:
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bCriticalHit);

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};
