// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/F1PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/F1TeamOutlineInterface.h"
#include "Character/F1CharacterBase.h"
#include "Input/F1InputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"

AF1PlayerController::AF1PlayerController()
{
	bReplicates = true;
}

void AF1PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

    if (GetWorld()->GetTimeSeconds() - LastCursorTraceTime >= CursorTraceInterval)
    {
        CursorTrace();
        LastCursorTraceTime = GetWorld()->GetTimeSeconds();
    }
}

void AF1PlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(F1Context);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
    {
        Subsystem->AddMappingContext(F1Context, 0);
    }

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

    UF1InputComponent* F1InputComponent = CastChecked<UF1InputComponent>(InputComponent);
    F1InputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AF1PlayerController::Move);
    F1InputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
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

    if (!CursorHit.bBlockingHit)
    {
        return;
    }

    AActor* HitActor = CursorHit.GetActor();
    LastActor = ThisActor;
    ThisActor = nullptr;

    // 안전한 캐스팅 및 로깅
    if (HitActor)
    {
        if (AF1CharacterBase* HitCharacter = Cast<AF1CharacterBase>(HitActor))
        {
            ThisActor = HitCharacter;
        }
    }

    if (LastActor == nullptr)
    {
        if (ThisActor != nullptr)
        {
            ThisActor->HighlightActor();
        }
    }
    else
    {
        if (ThisActor == nullptr)
        {
            LastActor->UnHighlightActor();
        }
        else
        {
            if (LastActor != ThisActor)
            {
                LastActor->UnHighlightActor();
                ThisActor->HighlightActor();
            }
        }
    }
}

UF1AbilitySystemComponent* AF1PlayerController::GetASC()
{
    if (F1AbilitySystemComponent == nullptr)
    {
        F1AbilitySystemComponent = Cast<UF1AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
    }
    return F1AbilitySystemComponent;
}

void AF1PlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    //GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
}

void AF1PlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    //GEngine->AddOnScreenDebugMessage(3, 3.f, FColor::Green, *InputTag.ToString());
    if (GetASC() == nullptr) return;
    GetASC()->AbilityInputTagHeld(InputTag);
}

void AF1PlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
    //GEngine->AddOnScreenDebugMessage(2, 3.f, FColor::Blue, *InputTag.ToString());
    if (GetASC() == nullptr) return;
    GetASC()->AbilityInputTagReleased(InputTag);
}