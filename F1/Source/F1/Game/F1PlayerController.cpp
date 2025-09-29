// Fill out your copyright notice in the Description page of Project Settings.

#include "Game/F1PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/F1TeamOutlineInterface.h"
#include "Character/F1CharacterBase.h"
#include "Input/F1InputComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "GameplayTag/F1GameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Net/UnrealNetwork.h"

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
        if (GetWorld()->GetTimeSeconds() - LastCursorTraceTime >= CursorTraceInterval)
        {
            CursorTrace();
            LastCursorTraceTime = GetWorld()->GetTimeSeconds();
        }

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

ETeamAttitude::Type AF1PlayerController::GetTeamAttitudeTowards(const AActor* Actor) const
{
    const APawn* MyPawn = GetPawn();
    if (!MyPawn || !Actor) return ETeamAttitude::Neutral;

    const IGenericTeamAgentInterface* MyTeam = Cast<IGenericTeamAgentInterface>(MyPawn);
    const IGenericTeamAgentInterface* TargetTeam = Cast<IGenericTeamAgentInterface>(Actor);

    if (!MyTeam || !TargetTeam) return ETeamAttitude::Neutral;

    return FGenericTeamId::GetAttitude(MyTeam->GetGenericTeamId(), TargetTeam->GetGenericTeamId());
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

            const float CharacterZ = StartLocation.Z;
            for (int32 i = 0; i < NavPath->PathPoints.Num(); i++)
            {
                FVector PointLoc = NavPath->PathPoints[i];
                PointLoc.Z = CharacterZ;
                Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
            }

            CachedDestination = NavPath->PathPoints.Last();
            CachedDestination.Z = CharacterZ;
            bAutoRunning = true;
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
        const FVector CurrentLocation = ControlledPawn->GetActorLocation();
        FVector Direction = (CachedDestination - CurrentLocation).GetSafeNormal();
        Direction.Z = 0;

        ControlledPawn->AddMovementInput(Direction);

        const float DistanceToDestination = FVector::Dist2D(CurrentLocation, CachedDestination);

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

    AActor* HitActor = CursorHit.GetActor();
    LastActor = ThisActor;
    ThisActor = nullptr;

    if (HitActor)
    {
        if (AF1CharacterBase* HitCharacter = Cast<AF1CharacterBase>(HitActor))
            ThisActor = HitCharacter;
    }

    if (LastActor != ThisActor)
    {
        if (LastActor) LastActor->UnHighlightActor();
        if (ThisActor) ThisActor->HighlightActor();
    }
}

UF1AbilitySystemComponent* AF1PlayerController::GetASC()
{
    if (F1AbilitySystemComponent == nullptr)
        F1AbilitySystemComponent = Cast<UF1AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
    return F1AbilitySystemComponent;
}

void AF1PlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
    if (InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_RMB))
    {
        bTargeting = ThisActor ? true : false;
        bAutoRunning = false;
    }
    else
    {
        if (GetASC())
        {
            GetASC()->AbilityInputTagPressed(InputTag);
        }
    }
}

void AF1PlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    if (!InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_RMB)) return;

    if (bTargeting)
    {
        if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
    }
    else
    {
        if (bAutoRunning) bAutoRunning = false;

        FollowTime += GetWorld()->GetDeltaSeconds();
        if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;

        if (APawn* ControlledPawn = GetPawn())
        {
            const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
            ControlledPawn->AddMovementInput(WorldDirection);
        }
    }
}

void AF1PlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
    if (!InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_RMB))
    {
        if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
        return;
    }

    if (bTargeting && ThisActor)
    {
        if (IsEnemy(ThisActor))
        {
            // 적군/중립 → 공격
        }
        else
        {
            // 아군 → 이동
            CachedDestination = ThisActor->GetActorLocation();
            StartMovementToDestination();
        }
    }
    else
    {
        // 빈 땅 클릭 → 이동
        const APawn* ControlledPawn = GetPawn();
        if (FollowTime <= ShortPressThreshold && ControlledPawn)
        {
            StartMovementToDestination();
        }
    }

    FollowTime = 0.f;
    bTargeting = false;
}

bool AF1PlayerController::IsEnemy(const AActor* Actor) const
{
    ETeamAttitude::Type Attitude = GetTeamAttitudeTowards(Actor);
    return Attitude == ETeamAttitude::Hostile || Attitude == ETeamAttitude::Neutral;
}
