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
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    UFUNCTION(BlueprintCallable, Category = "Target Projectile")
    void HandleRangeAndCast(const FVector& TargetLocation);

    UFUNCTION(BlueprintImplementableEvent, Category = "Target Projectile")
    void K2_ExecuteCast(const FVector& TargetLocation);

    // 자동 이동 관련 함수들
    UFUNCTION()
    void StartAutoMovement(const FVector& MoveLocation, const FVector& CastLocation);

    UFUNCTION()
    void StopAutoMovement();

    UFUNCTION()
    void CheckAutoMoveProgress();

    UFUNCTION(Server, Reliable)
    void Server_ExecuteCast(const FVector& TargetLocation);

private:
    UPROPERTY()
    bool bIsAutoMoving = false;

    UPROPERTY()
    FVector AutoMoveCastLocation;

    UPROPERTY()
    FTimerHandle AutoMoveCheckTimer;

    UPROPERTY(EditAnywhere, Category = "Auto Move")
    float MovementAcceptanceRadius = 50.0f;

    UPROPERTY()
    FVector AutoMoveTargetLocation;

    bool IsWithinRange(const FVector& TargetLocation) const;
    FVector GetRangeLocation(const FVector& TargetLocation) const;
    void ExecuteCast(const FVector& TargetLocation);
    void MoveToRangeAndCast(const FVector& TargetLocation);
};
