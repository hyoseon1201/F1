// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Ability/Projectile/F1NonTargetProjectileSpell.h"

void UF1NonTargetProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ExecuteCast();
}

void UF1NonTargetProjectileSpell::ExecuteCast()
{
	const FVector TargetLocation = CalculateMaxRangeLocation();
	K2_ExecuteCast(TargetLocation);
}

FVector UF1NonTargetProjectileSpell::GetMouseWorldDirection() const
{
	APlayerController* PC = GetActorInfo().PlayerController.Get();
	if (!PC)
	{
		return GetAvatarActorFromActorInfo()->GetActorForwardVector();
	}

	FVector WorldLocation, WorldDirection;
	PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	FVector CharacterLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector MouseWorldPos = WorldLocation + (WorldDirection * 2000.0f);

	FVector Direction = (MouseWorldPos - CharacterLocation).GetSafeNormal();
	Direction.Z = 0.0f;

	return Direction.GetSafeNormal();
}

FVector UF1NonTargetProjectileSpell::CalculateMaxRangeLocation() const
{
	const FVector CharacterLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector Direction = GetMouseWorldDirection();
	return CharacterLocation + (Direction * AbilityRange);
}
