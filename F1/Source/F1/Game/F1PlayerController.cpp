// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/F1PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/F1TeamOutlineInterface.h"
#include "Character/F1CharacterBase.h"

AF1PlayerController::AF1PlayerController()
{
	bReplicates = true;
}

void AF1PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
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

void AF1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AF1PlayerController::Move);
}

void AF1PlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AF1PlayerController::CursorTrace()
{
    FHitResult CursorHit;
    GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
    UE_LOG(LogTemp, Warning, TEXT("CursorTrace called"));

    if (!CursorHit.bBlockingHit)
    {
        UE_LOG(LogTemp, Warning, TEXT("No blocking hit"));
        return;
    }

    AActor* HitActor = CursorHit.GetActor();
    UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), HitActor ? *HitActor->GetName() : TEXT("None"));

    LastActor = ThisActor;
    ThisActor = nullptr;

    // 안전한 캐스팅 및 로깅
    if (HitActor)
    {
        if (AF1CharacterBase* HitCharacter = Cast<AF1CharacterBase>(HitActor))
        {
            ThisActor = HitCharacter;
            UE_LOG(LogTemp, Warning, TEXT("Cast to AF1CharacterBase SUCCESS: %s"), *HitCharacter->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Cast to AF1CharacterBase FAILED for: %s"), *HitActor->GetName());
        }
    }

    if (LastActor == nullptr)
    {
        if (ThisActor != nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Case B: Highlighting %s"), *ThisActor->GetName());
            ThisActor->HighlightActor();
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Case A: Both actors null"));
        }
    }
    else // LastActor is valid
    {
        if (ThisActor == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("Case C: Unhighlighting %s"), *LastActor->GetName());
            LastActor->UnHighlightActor();
        }
        else // both actors are valid
        {
            if (LastActor != ThisActor)
            {
                UE_LOG(LogTemp, Error, TEXT("Case D: Switch from %s to %s"),
                    *LastActor->GetName(), *ThisActor->GetName());
                LastActor->UnHighlightActor();
                ThisActor->HighlightActor();
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("Case E: Same actor, no change"));
            }
        }
    }
}


