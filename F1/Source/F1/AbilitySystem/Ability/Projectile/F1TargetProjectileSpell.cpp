// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Ability/Projectile/F1TargetProjectileSpell.h"

void UF1TargetProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] ActivateAbility called"));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UF1TargetProjectileSpell::HandleRangeAndCast(const FVector& TargetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] HandleRangeAndCast - TargetLocation: %s"), *TargetLocation.ToString());

	const FVector CharacterLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float Distance = FVector::Dist2D(CharacterLocation, TargetLocation);

	UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] CharacterLocation: %s"), *CharacterLocation.ToString());
	UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] Distance: %f, AbilityRange: %f"), Distance, AbilityRange);

	if (IsWithinRange(TargetLocation))
	{
		UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] Target is within range - ExecuteCast"));
		ExecuteCast(TargetLocation);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] Target is out of range - MoveToRangeAndCast"));
		MoveToRangeAndCast(TargetLocation);
	}
}

bool UF1TargetProjectileSpell::IsWithinRange(const FVector& TargetLocation) const
{
	const FVector CharacterLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float Distance = FVector::Dist2D(CharacterLocation, TargetLocation);
	const bool bIsWithinRange = Distance <= AbilityRange;

	UE_LOG(LogTemp, Log, TEXT("[TargetProjectileSpell] IsWithinRange - Distance: %f <= Range: %f = %s"),
		Distance, AbilityRange, bIsWithinRange ? TEXT("TRUE") : TEXT("FALSE"));

	return bIsWithinRange;
}

FVector UF1TargetProjectileSpell::GetRangeLocation(const FVector& TargetLocation) const
{
	const FVector CharacterLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector Direction = (TargetLocation - CharacterLocation).GetSafeNormal2D();
	const FVector RangeLocation = CharacterLocation + (Direction * (AbilityRange - 50.0f));

	UE_LOG(LogTemp, Log, TEXT("[TargetProjectileSpell] GetRangeLocation - Result: %s"), *RangeLocation.ToString());

	return RangeLocation;
}

void UF1TargetProjectileSpell::ExecuteCast(const FVector& TargetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] ExecuteCast - Calling K2_ExecuteCast with Location: %s"), *TargetLocation.ToString());
	K2_ExecuteCast(TargetLocation);
}

void UF1TargetProjectileSpell::MoveToRangeAndCast(const FVector& TargetLocation)
{
	const FVector MoveToLocation = GetRangeLocation(TargetLocation);
	UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] MoveToRangeAndCast - MoveLocation: %s, CastLocation: %s"),
		*MoveToLocation.ToString(), *TargetLocation.ToString());
	K2_MoveToRangeAndCast(MoveToLocation, TargetLocation);
}
