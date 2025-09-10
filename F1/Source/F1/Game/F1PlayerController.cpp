// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/F1PlayerController.h"
#include "EnhancedInputSubsystems.h"

AF1PlayerController::AF1PlayerController()
{
	bReplicates = true;
}

void AF1PlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(F1Context);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);

	Subsystem->AddMappingContext(F1Context, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}
