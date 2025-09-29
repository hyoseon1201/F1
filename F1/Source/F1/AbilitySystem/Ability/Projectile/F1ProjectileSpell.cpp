// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Ability/Projectile/F1ProjectileSpell.h"
#include "AbilitySystem/Actor/F1Projectile.h"
#include "Interaction/F1CombatInterface.h"

void UF1ProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("[ProjectileSpell] SpawnProjectile called - HasAuthority: %s, Location: %s"),
		bIsServer ? TEXT("TRUE") : TEXT("FALSE"), *ProjectileTargetLocation.ToString());

	if (!bIsServer)
	{
		UE_LOG(LogTemp, Error, TEXT("[ProjectileSpell] SpawnProjectile failed - No server authority"));
		return;
	}

	const FVector SocketLocation = GetProjectileSpawnLocation();
	const FRotator Rotation = GetProjectileRotation(ProjectileTargetLocation);

	UE_LOG(LogTemp, Warning, TEXT("[ProjectileSpell] Spawning projectile at: %s, targeting: %s"),
		*SocketLocation.ToString(), *ProjectileTargetLocation.ToString());

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
