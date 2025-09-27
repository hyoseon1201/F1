// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/Projectile/F1ProjectileSpell.h"
#include "F1TargetProjectileSpell.generated.h"

UCLASS()
class F1_API UF1TargetProjectileSpell : public UF1ProjectileSpell
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category = "Target Projectile")
	void HandleRangeAndCast(const FVector& TargetLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Target Projectile")
	void K2_ExecuteCast(const FVector& TargetLocation);

    // 자동 이동 처리 함수들
    UFUNCTION()
    void StartAutoMovement(const FVector& MoveLocation, const FVector& CastLocation);

    UFUNCTION()
    void StopAutoMovement();

    UFUNCTION()
    void OnAutoMoveCompleted();

    // 입력 감지 및 취소 처리
    UFUNCTION()
    void OnPlayerInputDetected();

private:
	bool IsWithinRange(const FVector& TargetLocation) const;
	FVector GetRangeLocation(const FVector& TargetLocation) const;
	void ExecuteCast(const FVector& TargetLocation);
	void MoveToRangeAndCast(const FVector& TargetLocation);

    UPROPERTY()
    bool bIsAutoMoving = false;

    UPROPERTY()
    FVector AutoMoveTargetLocation;

    UPROPERTY()
    FVector AutoMoveCastLocation;

    UPROPERTY()
    FTimerHandle AutoMoveCheckTimer;

    UPROPERTY(EditAnywhere, Category = "Auto Move")
    float MovementAcceptanceRadius = 50.0f;
};
