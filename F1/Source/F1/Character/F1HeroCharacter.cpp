// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/F1HeroCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Game/F1PlayerState.h"
#include "AbilitySystemComponent.h"
#include "Game/F1PlayerController.h"
#include "UI/HUD/F1HUD.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"

AF1HeroCharacter::AF1HeroCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// TEMP
	SetGenericTeamId(FGenericTeamId(1));
}

void AF1HeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
}

void AF1HeroCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void AF1HeroCharacter::InitAbilityActorInfo()
{
	AF1PlayerState* F1PlayerState = GetPlayerState<AF1PlayerState>();
	check(F1PlayerState);

	F1PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(F1PlayerState, this);
	Cast<UF1AbilitySystemComponent>(F1PlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = F1PlayerState->GetAbilitySystemComponent();
	AttributeSet = F1PlayerState->GetAttributeSet();

	if (IsLocallyControlled())
	{
		if (AF1PlayerController* F1PlayerController = Cast<AF1PlayerController>(GetController()))
		{
			if (AF1HUD* F1HUD = Cast<AF1HUD>(F1PlayerController->GetHUD()))
			{
				F1HUD->InitOverlay(F1PlayerController, F1PlayerState, AbilitySystemComponent, AttributeSet);
			}
		}
	}

	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
}
