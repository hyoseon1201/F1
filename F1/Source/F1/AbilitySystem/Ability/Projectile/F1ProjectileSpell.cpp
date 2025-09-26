// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Ability/Projectile/F1ProjectileSpell.h"
#include "AbilitySystem/Actor/F1Projectile.h"
#include "Interaction/F1CombatInterface.h"

void UF1ProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	const FVector SocketLocation = GetProjectileSpawnLocation();
	const FRotator Rotation = GetProjectileRotation(ProjectileTargetLocation);

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	AF1Projectile* Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	ApplyDamageEffect(Projectile);

	Projectile->FinishSpawning(SpawnTransform);
}

FVector UF1ProjectileSpell::GetProjectileSpawnLocation() const
{
	if (IF1CombatInterface* F1CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo()))
	{
		return F1CombatInterface->GetCombatSocketLocation();
	}
	return GetAvatarActorFromActorInfo()->GetActorLocation();
}

FRotator UF1ProjectileSpell::GetProjectileRotation(const FVector& TargetLocation) const
{
	const FVector SocketLocation = GetProjectileSpawnLocation();
	FRotator Rotation = (TargetLocation - SocketLocation).Rotation();
	Rotation.Pitch = 0.f;
	return Rotation;
}

void UF1ProjectileSpell::ApplyDamageEffect(AF1Projectile* Projectile)
{
	// TODO : 프로젝타일에 GE 부여
	// Projectile->SetDamage(BaseDamage);
}
