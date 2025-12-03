// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/F1DamageGameplayAbility.h"
#include "F1ProjectileSpell.generated.h"

class AF1Projectile;

/**
 * 투사체 발사 어빌리티 (직사, 유도, 곡사 지원)
 */
UCLASS()
class F1_API UF1ProjectileSpell : public UF1DamageGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 1. 직사 (직선 발사)
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);

	// 2. 유도 (타겟 추적)
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnHomingProjectile(AActor* HomingTarget);

	// 3. 곡사 (포물선)
	// OverrideGravityZ: 0이면 투사체의 GravityScale을 반영한 월드 중력 사용
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnArcProjectile(const FVector& TargetLocation, float OverrideGravityZ = 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AF1Projectile> ProjectileClass;
};