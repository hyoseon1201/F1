// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Ability/Projectile/F1TargetProjectileSpell.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Game/F1PlayerController.h"

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

void UF1TargetProjectileSpell::StartAutoMovement(const FVector& MoveLocation, const FVector& CastLocation)
{
	StopAutoMovement();

	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	AF1PlayerController* F1PlayerController = Cast<AF1PlayerController>(AvatarPawn->GetController());

	if (!F1PlayerController) return;

	bIsAutoMoving = true;
	AutoMoveTargetLocation = MoveLocation;
	AutoMoveCastLocation = CastLocation;

	// ��������Ʈ ���ε� (�̵� �Ϸ� �� �ڵ� ȣ���)
	F1PlayerController->OnAutoRunCompleted.AddDynamic(this, &UF1TargetProjectileSpell::OnAutoMoveCompleted);

	// PlayerController �̵� ����
	F1PlayerController->StartAbilityMovementToDestination(MoveLocation);

	UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] Auto movement started with delegate"));
}

void UF1TargetProjectileSpell::StopAutoMovement()
{
	if (!bIsAutoMoving) return;

	bIsAutoMoving = false;

	// ��������Ʈ ����ε�
	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (AF1PlayerController* F1PlayerController = Cast<AF1PlayerController>(AvatarPawn->GetController()))
	{
		F1PlayerController->OnAutoRunCompleted.RemoveDynamic(this, &UF1TargetProjectileSpell::OnAutoMoveCompleted);
	}

	UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] Auto movement stopped"));
}

void UF1TargetProjectileSpell::OnAutoMoveCompleted()
{
	UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] Auto movement completed, executing cast"));

	// �ڵ� �̵� ���� ����
	StopAutoMovement();

	// ��ų ����
	ExecuteCast(AutoMoveCastLocation);
}

void UF1TargetProjectileSpell::OnPlayerInputDetected()
{
	if (bIsAutoMoving)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] Player input detected, canceling auto movement"));
		StopAutoMovement();

		// �ʿ�� Ability ��ü�� ������ ���� ����
		// EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
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
	// ���� ���� üũ
	if (!GetAvatarActorFromActorInfo()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] MoveToRangeAndCast called without authority"));
		return;
	}

	const FVector MoveToLocation = GetRangeLocation(TargetLocation);
	UE_LOG(LogTemp, Warning, TEXT("[TargetProjectileSpell] Starting auto movement to: %s for casting at: %s"),
		*MoveToLocation.ToString(), *TargetLocation.ToString());

	// �ڵ� �̵� ����
	StartAutoMovement(MoveToLocation, TargetLocation);
}
