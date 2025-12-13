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
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GenericTeamAgentInterface.h"

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

        TraceAndAttackTarget();
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
    if (GetASC())
    {
        FGameplayTagContainer CancelTags;
        CancelTags.AddTag(FGameplayTag::RequestGameplayTag("Ability"));
        GetASC()->CancelAbilities(&CancelTags);
    }

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

void AF1PlayerController::TraceAndAttackTarget()
{
    // 타겟이 없으면 리턴
    if (!TargetEnemy) return;

    // 타겟이 죽었거나 사라졌으면 추적 중지
    // (죽었는지 확인하는 로직은 Interface나 Tag로 추가 확인 권장)
    if (!IsValid(TargetEnemy))
    {
        TargetEnemy = nullptr;
        return;
    }

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn) return;

    if (GetASC())
    {
        // "Ability.State.Attacking" 태그가 있으면(즉, 공격 모션 중이면) 리턴
        // 혹은 쿨타임 태그가 있어도 리턴하게 하면 더 좋습니다.
        FGameplayTag AttackingTag = FGameplayTag::RequestGameplayTag("Ability.State.Attacking");
        FGameplayTag CooldownTag = FGameplayTag::RequestGameplayTag("Cooldown.Attack"); // 본인 쿨타임 태그

        if (GetASC()->HasMatchingGameplayTag(AttackingTag) ||
            GetASC()->HasMatchingGameplayTag(CooldownTag))
        {
            return; // "아직 공격 안 끝났거나 쿨타임이다. 진정해라."
        }
    }

    // 1. 거리 계산
    float Distance = ControlledPawn->GetDistanceTo(TargetEnemy);

    // 공격 사거리 (나중에는 스탯에서 가져와야 함. 예: 600.f)
    // 약간의 여유(-50)를 둬서 사거리 끝자락에서 버벅이지 않게 함
    float AttackRange = 600.0f;

    if (Distance <= AttackRange - 50.0f)
    {
        // A. 사거리 안: 멈추고 공격!
        StopMovement(); // 이동 멈춤

        // 회전: 적을 바라보게 함
        FVector LookAt = TargetEnemy->GetActorLocation() - ControlledPawn->GetActorLocation();
        LookAt.Z = 0.f;
        ControlledPawn->SetActorRotation(LookAt.Rotation());

        // B. GAS 어빌리티 발동
        if (GetASC())
        {
            // Payload에 타겟 정보를 실어서 보냄
            FGameplayEventData Payload;
            Payload.Instigator = ControlledPawn;
            Payload.Target = TargetEnemy;

            // 태그로 어빌리티 발동 시도 (GA_Attack에 Trigger Tag로 Ability.Attack 설정 필요)
            // TryActivateAbilitiesByTag는 Payload를 못 보내므로, SendGameplayEventToActor 사용 권장

            // 방법 1: Event로 발동 (추천) -> GA_Attack의 Trigger를 'GameplayEvent'로 설정
            UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
                ControlledPawn,
                FF1GameplayTags::Get().Event_Combat_Attack,
                Payload
            );
        }
    }
    else
    {
        // C. 사거리 밖: 추적 이동
        // Spline 이동이 아니라 AI Navigation 이동을 사용해야 움직이는 적을 잘 쫓아감
        UAIBlueprintHelperLibrary::SimpleMoveToActor(this, TargetEnemy);
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
    TargetEnemy = nullptr;
}

void AF1PlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
    // RMB가 아닌 다른 입력은 AbilitySystem으로 전달
    if (!InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_RMB))
    {
        if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
        return;
    }

    TargetEnemy = nullptr;

    if (GetASC())
    {
        FGameplayTagContainer CancelTags;
        CancelTags.AddTag(FGameplayTag::RequestGameplayTag("Ability")); // 모든 어빌리티 취소

        GetASC()->CancelAbilities(&CancelTags);
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
        bool bTargetIsEnemy = false;
        // 1. 커서 아래에 캐릭터가 있을 때만 판별
        if (ThisActor)
        {
            // A. 나(플레이어)와 타겟(적)을 인터페이스로 캐스팅
            // CharacterBase가 이미 IGenericTeamAgentInterface를 상속받았다고 가정합니다.
            IGenericTeamAgentInterface* MyTeamAgent = Cast<IGenericTeamAgentInterface>(GetPawn());
            IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(ThisActor);

            // B. 둘 다 팀 정보가 있을 때만 비교
            if (MyTeamAgent && TargetTeamAgent)
            {
                // FGenericTeamId는 ==, != 연산자가 오버로딩 되어 있어서 바로 비교 가능합니다.
                // GetGenericTeamId()는 FGenericTeamId 타입을 반환합니다.
                FGenericTeamId MyTeamID = MyTeamAgent->GetGenericTeamId();
                FGenericTeamId TargetTeamID = TargetTeamAgent->GetGenericTeamId();

                // [핵심] 팀이 다르고, 타겟이 '중립(NoTeam)'이 아니라면 적으로 간주
                if (MyTeamID != TargetTeamID && TargetTeamID != FGenericTeamId::NoTeam)
                {
                    bTargetIsEnemy = true;
                }
            }
            // 만약 Pawn에 없고 PlayerState에만 있다면? (혹시 위 캐스팅이 실패할 경우 대비)
            else
            {
                // 안전장치: Pawn이 아니라 PlayerState에서 찾아야 하는 경우
                // 보통 CharacterBase에서 GetGenericTeamId를 오버라이드해서 
                // "내 PlayerState의 ID를 리턴해라"라고 짜두는 게 정석입니다.
                // 작성하신 CharacterBase에 그렇게 되어 있다면 위의 A 로직으로 충분합니다.
            }
        }

        if (bTargetIsEnemy)
        {
            // [추적 모드 ON] -> 적입니다!
            TargetEnemy = ThisActor;
            bAutoRunning = false;
            Spline->ClearSplinePoints();
        }
        else
        {
            // [일반 이동] -> 땅이거나 아군입니다.
            TargetEnemy = nullptr;

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
    }

    FollowTime = 0.f;
}

void AF1PlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bCriticalHit)
{
    if (IsValid(TargetCharacter) && DamageTextComponentClass)
    {
        UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
        DamageText->RegisterComponent();
        DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        DamageText->SetDamageText(DamageAmount, bCriticalHit);
    }
}
