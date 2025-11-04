// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/F1PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/F1CharacterBase.h"
#include "Input/F1InputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "GameplayTag/F1GameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"

AF1PlayerController::AF1PlayerController()
{
    bReplicates = true;
    Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AF1PlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);

    if (IsLocalPlayerController())
    {
        CursorTrace(); // Aura 방식: 매 틱마다 커서 트레이스
        AutoRun();
    }
}

void AF1PlayerController::BeginPlay()
{
    Super::BeginPlay();
    check(F1Context);

    UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    if (Subsystem) Subsystem->AddMappingContext(F1Context, 0);

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
    F1InputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AF1PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AF1PlayerController, bAutoRunning);
    DOREPLIFETIME(AF1PlayerController, CachedDestination);
}

void AF1PlayerController::StartMovementToDestination()
{
    const APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    const FVector StartLocation = ControlledPawn->GetActorLocation();

    if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
        this, StartLocation, CachedDestination))
    {
        if (NavPath->PathPoints.Num() > 0)
        {
            Spline->ClearSplinePoints();

            for (const FVector& PointLoc : NavPath->PathPoints)
            {
                Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
            }

            if (NavPath->PathPoints.Num() > 0)
            {
                CachedDestination = NavPath->PathPoints.Last();
                bAutoRunning = true;
            }
        }
    }
}

void AF1PlayerController::StartAbilityMovementToDestination(const FVector& Destination)
{
    CachedDestination = Destination;
    StartMovementToDestination();
}

void AF1PlayerController::AutoRun()
{
    if (!bAutoRunning) return;

    if (APawn* ControlledPawn = GetPawn())
    {
        // Aura 방식: Spline 경로 기반 이동
        const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
            ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
        const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
            LocationOnSpline, ESplineCoordinateSpace::World);

        ControlledPawn->AddMovementInput(Direction);

        const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
        if (DistanceToDestination <= AutoRunAcceptanceRadius)
        {
            bAutoRunning = false;
        }
    }
}

void AF1PlayerController::CursorTrace()
{
    GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
    if (!CursorHit.bBlockingHit) return;

    LastActor = ThisActor;
    ThisActor = nullptr;

    if (AActor* HitActor = CursorHit.GetActor())
    {
        if (AF1CharacterBase* HitCharacter = Cast<AF1CharacterBase>(HitActor))
        {
            ThisActor = HitCharacter;
        }
    }

    // 하이라이트 처리
    if (LastActor != ThisActor)
    {
        if (LastActor) LastActor->UnHighlightActor();
        if (ThisActor) ThisActor->HighlightActor();
    }
}

UF1AbilitySystemComponent* AF1PlayerController::GetASC()
{
    if (F1AbilitySystemComponent == nullptr)
    {
        F1AbilitySystemComponent = Cast<UF1AbilitySystemComponent>(
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
    }
    return F1AbilitySystemComponent;
}

void AF1PlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    // RMB가 아닌 다른 입력은 AbilitySystem으로 전달
    if (!InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_RMB))
    {
        if (GetASC())
        {
            GetASC()->AbilityInputTagPressed(InputTag);
        }

        if (InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_LMB) ||
            InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_Q) ||
            InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_W) ||
            InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_E) ||
            InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_R))
        {
            bAutoRunning = false;
        }

        return;
    }

    // RMB Pressed: 자동 이동 중지
    bAutoRunning = false;
}

void AF1PlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    // RMB가 아닌 다른 입력은 AbilitySystem으로 전달
    if (!InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_RMB))
    {
        if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
        return;
    }

    // RMB Held: 드래그 이동
    FollowTime += GetWorld()->GetDeltaSeconds();

    if (CursorHit.bBlockingHit)
    {
        CachedDestination = CursorHit.ImpactPoint;
    }

    if (APawn* ControlledPawn = GetPawn())
    {
        const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
        ControlledPawn->AddMovementInput(WorldDirection);
    }
}

void AF1PlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
    // RMB가 아닌 다른 입력은 AbilitySystem으로 전달
    if (!InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_RMB))
    {
        if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
        return;
    }

    // 짧은 클릭인 경우에만 이동 명령 실행
    if (FollowTime <= ShortPressThreshold)
    {
        const APawn* ControlledPawn = GetPawn();
        if (ControlledPawn)
        {
            // 커서 아래에 액터가 있으면 그 위치로, 없으면 땅 위치로 이동
            if (ThisActor)
            {
                CachedDestination = ThisActor->GetActorLocation();
            }
            else
            {
                CachedDestination = CursorHit.ImpactPoint;
            }

            StartMovementToDestination();
        }
    }

    FollowTime = 0.f;
}

void AF1PlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter)
{
    if (IsValid(TargetCharacter) && DamageTextComponentClass)
    {
        UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
        DamageText->RegisterComponent();
        DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        DamageText->SetDamageText(DamageAmount);
    }
}
