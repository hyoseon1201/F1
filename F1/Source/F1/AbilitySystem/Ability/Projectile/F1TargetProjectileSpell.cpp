// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Ability/Projectile/F1TargetProjectileSpell.h"
#include "Game/F1PlayerController.h"

void UF1TargetProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UF1TargetProjectileSpell::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    StopAutoMovement();
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UF1TargetProjectileSpell::HandleRangeAndCast(const FVector& TargetLocation)
{
    if (IsWithinRange(TargetLocation))
    {
        ExecuteCast(TargetLocation);
    }
    else
    {
        MoveToRangeAndCast(TargetLocation);
    }
}

void UF1TargetProjectileSpell::MoveToRangeAndCast(const FVector& TargetLocation)
{
    if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

    const FVector MoveToLocation = GetRangeLocation(TargetLocation);
    StartAutoMovement(MoveToLocation, TargetLocation);
}

void UF1TargetProjectileSpell::StartAutoMovement(const FVector& MoveLocation, const FVector& CastLocation)
{
    StopAutoMovement();

    APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
    AF1PlayerController* PlayerController = Cast<AF1PlayerController>(AvatarPawn->GetController());

    if (!AvatarPawn || !PlayerController) return;

    bIsAutoMoving = true;
    AutoMoveTargetLocation = MoveLocation;
    AutoMoveCastLocation = CastLocation;

    PlayerController->StartAbilityMovementToDestination(MoveLocation);

    if (GetAvatarActorFromActorInfo()->HasAuthority())
    {
        GetWorld()->GetTimerManager().SetTimer(
            AutoMoveCheckTimer,
            this,
            &UF1TargetProjectileSpell::CheckAutoMoveProgress,
            0.2f,
            true
        );
    }
}

void UF1TargetProjectileSpell::CheckAutoMoveProgress()
{
    if (!bIsAutoMoving) return;

    APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
    if (!AvatarPawn) return;

    const FVector CurrentLocation = AvatarPawn->GetActorLocation();
    const float DistanceToTarget = FVector::Dist2D(CurrentLocation, AutoMoveTargetLocation);

    if (DistanceToTarget <= 50.0f)
    {
        StopAutoMovement();
        ExecuteCast(AutoMoveCastLocation);
    }
}

void UF1TargetProjectileSpell::StopAutoMovement()
{
    if (!bIsAutoMoving) return;

    bIsAutoMoving = false;

    if (AutoMoveCheckTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(AutoMoveCheckTimer);
        AutoMoveCheckTimer.Invalidate();
    }
}

void UF1TargetProjectileSpell::ExecuteCast(const FVector& TargetLocation)
{
    if (GetAvatarActorFromActorInfo()->HasAuthority())
    {
        K2_ExecuteCast(TargetLocation);
    }
    else
    {
        Server_ExecuteCast(TargetLocation);
    }
}

void UF1TargetProjectileSpell::Server_ExecuteCast_Implementation(const FVector& TargetLocation)
{
    K2_ExecuteCast(TargetLocation);
}

bool UF1TargetProjectileSpell::IsWithinRange(const FVector& TargetLocation) const
{
    const FVector CharacterLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
    const float Distance = FVector::Dist2D(CharacterLocation, TargetLocation);
    return Distance <= AbilityRange;
}

FVector UF1TargetProjectileSpell::GetRangeLocation(const FVector& TargetLocation) const
{
    const FVector CharacterLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

    const float DX = TargetLocation.X - CharacterLocation.X;
    const float DY = TargetLocation.Y - CharacterLocation.Y;
    const float Distance = FMath::Sqrt(DX * DX + DY * DY);

    if (Distance <= AbilityRange)
    {
        return CharacterLocation;
    }

    const float TargetDistance = AbilityRange - 100.0f;
    const float UnitX = (CharacterLocation.X - TargetLocation.X) / Distance;
    const float UnitY = (CharacterLocation.Y - TargetLocation.Y) / Distance;

    return FVector(
        TargetLocation.X + (UnitX * TargetDistance),
        TargetLocation.Y + (UnitY * TargetDistance),
        CharacterLocation.Z
    );
}
