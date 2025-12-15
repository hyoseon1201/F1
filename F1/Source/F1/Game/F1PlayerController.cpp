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
#include "UI/Widget/DamageTextComponent.h" // 헤더 포함 확인!
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "Engine/OverlapResult.h"
#include "Interaction/F1TeamOutlineInterface.h"
#include "GameFramework/CharacterMovementComponent.h" // 회전 제어용
#include "Kismet/KismetMathLibrary.h" // 각도 계산용

AF1PlayerController::AF1PlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AF1PlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(F1Context);

	if (!PrimaryActorTick.IsTickFunctionRegistered())
	{
		PrimaryActorTick.bCanEverTick = true;
		PrimaryActorTick.RegisterTickFunction(GetLevel());
	}
	SetActorTickEnabled(true);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem) Subsystem->AddMappingContext(F1Context, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AF1PlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	SetActorTickEnabled(true);
}

void AF1PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 1. [서버] 두뇌 역할
	if (HasAuthority() || IsLocalPlayerController())
	{
		TraceAndAttackTarget();
	}

	// 2. [클라이언트] 행동 대장
	if (IsLocalPlayerController())
	{
		CursorTrace();
		AutoRun();

		// [회전] 부드러운 회전 로직 (공격 타겟 주시)
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn)
		{
			if (FaceTargetActor.IsValid())
			{
				if (UCharacterMovementComponent* CMC = ControlledPawn->FindComponentByClass<UCharacterMovementComponent>())
				{
					CMC->bOrientRotationToMovement = false; // 이동 회전 끄기
				}

				FVector MyLoc = ControlledPawn->GetActorLocation();
				FVector TargetLoc = FaceTargetActor->GetActorLocation();
				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(MyLoc, TargetLoc);
				FRotator TargetRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);

				// 부드럽게 회전
				FRotator NewRotation = FMath::RInterpTo(ControlledPawn->GetActorRotation(), TargetRotation, DeltaTime, 20.0f);
				ControlledPawn->SetActorRotation(NewRotation);
			}
			else
			{
				// 타겟 없으면 다시 이동 방향 보기
				if (UCharacterMovementComponent* CMC = ControlledPawn->FindComponentByClass<UCharacterMovementComponent>())
				{
					CMC->bOrientRotationToMovement = true;
				}
			}
		}
	}
}

void AF1PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UF1InputComponent* F1InputComponent = CastChecked<UF1InputComponent>(InputComponent);
	F1InputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

// ==============================================================================
//  RPC Implementation
// ==============================================================================

void AF1PlayerController::Server_SetTargetEnemy_Implementation(AActor* NewTarget)
{
	if (TargetEnemy == NewTarget) return;
	TargetEnemy = NewTarget;

	if (TargetEnemy)
	{
		StopMovement();
	}
}

void AF1PlayerController::Client_MoveToPoints_Implementation(const TArray<FVector>& PathPoints)
{
	Spline->ClearSplinePoints();
	for (const FVector& PointLoc : PathPoints)
	{
		Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
	}

	if (PathPoints.Num() > 0)
	{
		CachedDestination = PathPoints.Last();
		bAutoRunning = true;
	}
}

void AF1PlayerController::Client_FaceTarget_Implementation(AActor* TargetToFace)
{
	// 직접 회전시키지 않고 타겟만 설정 (Tick에서 부드럽게 회전)
	FaceTargetActor = TargetToFace;
}

void AF1PlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bCriticalHit)
{
	// [복구 완료] 데미지 텍스트 생성 로직
	if (IsValid(TargetCharacter) && DamageTextComponentClass)
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bCriticalHit);
	}
}

void AF1PlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AF1PlayerController, bAutoRunning);
	DOREPLIFETIME(AF1PlayerController, CachedDestination);
	DOREPLIFETIME(AF1PlayerController, TargetEnemy);
}

// ==============================================================================
//  Movement & Combat Logic
// ==============================================================================

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

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
		this, StartLocation, CachedDestination);

	if (NavPath && NavPath->PathPoints.Num() > 0)
	{
		if (HasAuthority())
		{
			bAutoRunning = true;
			Client_MoveToPoints(NavPath->PathPoints);
		}
		else
		{
			Spline->ClearSplinePoints();
			for (const FVector& PointLoc : NavPath->PathPoints)
			{
				Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
			}
			CachedDestination = NavPath->PathPoints.Last();
			bAutoRunning = true;
		}
	}
}

void AF1PlayerController::AutoRun()
{
	if (!bAutoRunning) return;

	if (APawn* ControlledPawn = GetPawn())
	{
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
	// 타겟이 없거나 유효하지 않으면 리턴
	if (!TargetEnemy || !IsValid(TargetEnemy))
	{
		TargetEnemy = nullptr;
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	// (옵션) 클라이언트 오토런 해제 조건
	//if (bAutoRunning && ControlledPawn->GetVelocity().SizeSquared() < 10.0f)
	//{
	//	bAutoRunning = false;
	//}

	float Distance = ControlledPawn->GetDistanceTo(TargetEnemy);
	float AttackRange = 600.0f; // 나중에 스탯 적용

	// ====================================================
	// 1. 공격 범위 안 (Attack Phase) -> [클라/서버 모두 실행]
	// ====================================================
	if (Distance <= AttackRange - 50.0f)
	{
		// [수정] 쿨타임 체크를 가장 먼저 해야 합니다!
		// GAS 컴포넌트에게 "공격 쿨타임 태그"가 붙어있는지 물어봅니다.
		if (UF1AbilitySystemComponent* MyASC = GetASC())
		{
			// 예: F1GameplayTags::Get().Cooldown_Combat_Attack
			// (님 프로젝트에 맞는 쿨타임 태그를 넣으세요)
			FGameplayTag CooldownTag = FF1GameplayTags::Get().Cooldown_Attack;

			if (MyASC->HasMatchingGameplayTag(CooldownTag))
			{
				// 쿨타임 중이다? -> 아무것도 하지 말고 리턴!
				// 이렇게 하면 아래의 StopMovement, Rotation 로직이 실행되지 않고
				// Tick의 뒷부분에 있는 AutoRun()이 계속 실행되어 무빙이 끊기지 않습니다.
				return;
			}
		}

		// --------------------------------------------------------
		// 여기서부터는 "공격 가능 상태"일 때만 실행됨
		// --------------------------------------------------------

		// 2. [이동 정지]
		if (HasAuthority())
		{
			Client_MoveToPoints(TArray<FVector>());
		}
		else
		{
			bAutoRunning = false;
			Spline->ClearSplinePoints();
			StopMovement();
		}

		// 3. [회전]
		FVector LookAt = TargetEnemy->GetActorLocation() - ControlledPawn->GetActorLocation();
		LookAt.Z = 0.f;
		ControlledPawn->SetActorRotation(LookAt.Rotation());

		if (HasAuthority()) Client_FaceTarget(TargetEnemy);
		else FaceTargetActor = TargetEnemy;

		// 4. [공격 명령]
		if (GetASC())
		{
			// (기존 LastAttackTime 체크는 GAS 쿨타임이 있으면 사실상 필요 없지만, 
			//  이중 안전장치로 두셔도 됩니다.)
			double CurrentTime = GetWorld()->GetTimeSeconds();
			if (CurrentTime - LastAttackTime < 0.1f) return; // 아주 짧게만

			LastAttackTime = CurrentTime;

			FGameplayEventData Payload;
			Payload.Instigator = ControlledPawn;
			Payload.Target = TargetEnemy;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				ControlledPawn,
				FF1GameplayTags::Get().Event_Combat_Attack,
				Payload
			);
		}
	}
	// ====================================================
	// 2. 공격 범위 밖 (Chasing Phase) -> [서버만 실행!]
	// ====================================================
	else
	{
		// ?? 중요: 이동/길찾기 로직은 '서버'만 수행해야 안전합니다.
		if (HasAuthority())
		{
			FVector EnemyLoc = TargetEnemy->GetActorLocation();
			FVector MyLoc = ControlledPawn->GetActorLocation();
			FVector DirectionToMe = (MyLoc - EnemyLoc).GetSafeNormal();
			FVector ShootingPos = EnemyLoc + (DirectionToMe * (AttackRange * 0.7f));

			FNavLocation ProjectedLoc;
			if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this))
			{
				if (NavSys->ProjectPointToNavigation(ShootingPos, ProjectedLoc))
				{
					ShootingPos = ProjectedLoc.Location;
				}
			}

			float DistToDest = FVector::DistSquared(CachedDestination, ShootingPos);

			// 목적지가 많이 바뀌었을 때만 경로 갱신
			if (!bAutoRunning || DistToDest > (100.0f * 100.0f))
			{
				CachedDestination = ShootingPos;
				StartMovementToDestination(); // 내부에서 RPC(Client_MoveToPoints)를 호출할 것임
			}
		}
	}
}

// ==============================================================================
//  Input Handling
// ==============================================================================

void AF1PlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_RMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);

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

	// [RMB] 이동 시작 시 회전 타겟 해제
	bAutoRunning = false;
	Server_SetTargetEnemy(nullptr);
	FaceTargetActor = nullptr; // 정면 보기
}

void AF1PlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_RMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	FollowTime += GetWorld()->GetDeltaSeconds();
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// [반응성 개선] 적 클릭 시에만 신중하게 대기
	if (CursorHit.bBlockingHit && IsEnemy(CursorHit.GetActor()))
	{
		if (FollowTime <= ShortPressThreshold) return;
	}

	// [즉시 이동] 땅 클릭이나 시간 지남
	Server_SetTargetEnemy(nullptr);
	FaceTargetActor = nullptr; // 정면 보기
	bAutoRunning = false;      // AutoRun 끄기
	Spline->ClearSplinePoints();

	if (GetASC())
	{
		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(FGameplayTag::RequestGameplayTag("Ability"));
		GetASC()->CancelAbilities(&CancelTags);
	}

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
	if (!InputTag.MatchesTagExact(FF1GameplayTags::Get().InputTag_RMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}

	if (FollowTime <= ShortPressThreshold)
	{
		AActor* AssistTarget = GetTargetUnderCursorWithAssist();

		if (AssistTarget)
		{
			// 적 타겟팅
			Server_SetTargetEnemy(AssistTarget);
			bAutoRunning = false;
			Spline->ClearSplinePoints();
		}
		else
		{
			// 땅 클릭 이동
			Server_SetTargetEnemy(nullptr);
			if (APawn* ControlledPawn = GetPawn())
			{
				if (ThisActor) CachedDestination = ThisActor->GetActorLocation();
				else CachedDestination = CursorHit.ImpactPoint;

				StartMovementToDestination();
			}
		}
	}
	FollowTime = 0.f;
}

// ==============================================================================
//  Helpers
// ==============================================================================

bool AF1PlayerController::IsEnemy(AActor* Target)
{
	if (!Target) return false;
	IGenericTeamAgentInterface* MyAgent = Cast<IGenericTeamAgentInterface>(GetPawn());
	IGenericTeamAgentInterface* TargetAgent = Cast<IGenericTeamAgentInterface>(Target);

	if (MyAgent && TargetAgent)
	{
		return MyAgent->GetGenericTeamId() != TargetAgent->GetGenericTeamId();
	}
	return false;
}

AActor* AF1PlayerController::GetTargetUnderCursorWithAssist()
{
	FHitResult Hit;
	GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	if (!Hit.bBlockingHit) return nullptr;

	AActor* HitActor = Hit.GetActor();
	if (HitActor && IsEnemy(HitActor)) return HitActor;

	TArray<FOverlapResult> Overlaps;
	FVector Center = Hit.ImpactPoint;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(TargetingAssistRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetPawn());

	if (GetWorld()->OverlapMultiByChannel(Overlaps, Center, FQuat::Identity, ECC_Pawn, Sphere, Params))
	{
		AActor* ClosestEnemy = nullptr;
		float MinDistSq = FLT_MAX;
		for (const FOverlapResult& Overlap : Overlaps)
		{
			AActor* OverlapActor = Overlap.GetActor();
			if (OverlapActor && IsEnemy(OverlapActor))
			{
				float DistSq = FVector::DistSquared(Center, OverlapActor->GetActorLocation());
				if (DistSq < MinDistSq)
				{
					MinDistSq = DistSq;
					ClosestEnemy = OverlapActor;
				}
			}
		}
		return ClosestEnemy;
	}
	return nullptr;
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

	if (LastActor != ThisActor)
	{
		if (LastActor)
		{
			if (IF1TeamOutlineInterface* OutlineInterface = Cast<IF1TeamOutlineInterface>(LastActor))
			{
				OutlineInterface->UnHighlightActor();
			}
		}
		if (ThisActor)
		{
			if (IF1TeamOutlineInterface* OutlineInterface = Cast<IF1TeamOutlineInterface>(ThisActor))
			{
				OutlineInterface->HighlightActor();
			}
		}
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