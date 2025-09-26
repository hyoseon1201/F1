// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/F1GameplayAbility.h"
#include "F1ProjectileSpell.generated.h"

class AF1Projectile;

UCLASS(Abstract)
class F1_API UF1ProjectileSpell : public UF1GameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AF1Projectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell Config")
	float AbilityRange = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	float ProjectileSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell Config")
	float BaseDamage = 50.0f;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	virtual void SpawnProjectile(const FVector& ProjectileTargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	virtual FVector GetProjectileSpawnLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	virtual FRotator GetProjectileRotation(const FVector& TargetLocation) const;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	virtual void ApplyDamageEffect(AF1Projectile* Projectile);
};
