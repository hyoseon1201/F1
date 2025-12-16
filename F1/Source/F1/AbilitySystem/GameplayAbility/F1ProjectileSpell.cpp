#include "AbilitySystem/GameplayAbility/F1ProjectileSpell.h"
#include "AbilitySystem/Actor/F1Projectile.h"
#include "Interaction/F1CombatInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/F1GameplayTags.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystem/AbilityTask/F1AT_WaitClientTargetData.h"
#include "DrawDebugHelpers.h"

void UF1ProjectileSpell::CacheTargetData(const FGameplayAbilityTargetDataHandle& DataHandle, EF1ProjectileSpawnMode OverrideSpawnMode)
{
	if (DataHandle.Data.IsValidIndex(0))
	{
		const FGameplayAbilityTargetData_SingleTargetHit* HitData = static_cast<const FGameplayAbilityTargetData_SingleTargetHit*>(DataHandle.Get(0));

		if (HitData && HitData->HitResult.IsValidBlockingHit())
		{
			// 1. 위치 및 타겟 저장
			CachedHomingTarget = HitData->HitResult.GetActor();
			CachedTargetLocation = HitData->HitResult.Location;

			// [핵심 수정] 
			// 인자로 받은 모드가 있으면 그걸 쓰고, 없으면(None) 자동으로 판단
			if (OverrideSpawnMode != EF1ProjectileSpawnMode::None)
			{
				SpawnMode = OverrideSpawnMode;
			}
			else
			{
				if (CachedHomingTarget && CachedHomingTarget->Implements<UF1CombatInterface>())
				{
					SpawnMode = EF1ProjectileSpawnMode::Homing;
				}
				else
				{
					SpawnMode = EF1ProjectileSpawnMode::Linear;
				}
			}

			UE_LOG(LogTemp, Warning, TEXT("[Ability] CacheTargetData: Location Saved %s, Mode: %d"), *CachedTargetLocation.ToString(), (int32)SpawnMode);
		}
	}
}

void UF1ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 주의: 여기서 바로 Task를 시작하지 않고, 
	// 블루프린트에서 SpawnProjectile... 노드를 호출할 때 시작하도록 설계됨.
}

void UF1ProjectileSpell::StartTargetDataTask(EF1TargetingType TargetingType)
{
	// [수정] 인자로 받은 TargetingType을 태스크 생성 시 넘겨줌
	UF1AT_WaitClientTargetData* Task = UF1AT_WaitClientTargetData::WaitClientTargetData(this, FName("WaitTargetData"), TargetingType);
	Task->ValidData.AddDynamic(this, &UF1ProjectileSpell::OnClientDataReceived);
	Task->ReadyForActivation();
}

// 1. 직사 발사 (보통 논타겟 -> Ground 모드)
void UF1ProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	SpawnMode = EF1ProjectileSpawnMode::Linear;
	CachedTargetLocation = ProjectileTargetLocation;

	// "나는 땅을 찍어도 발사할 거야"
	StartTargetDataTask(EF1TargetingType::Ground);
}

// 2. 유도 발사 (타겟팅 -> Enemy 모드)
void UF1ProjectileSpell::SpawnHomingProjectile(AActor* HomingTarget)
{
	SpawnMode = EF1ProjectileSpawnMode::Homing;
	CachedHomingTarget = HomingTarget;

	// "나는 적이 아니면 발사 안 해"
	StartTargetDataTask(EF1TargetingType::Enemy);
}

// 3. 곡사 발사 (보통 논타겟 -> Ground 모드)
void UF1ProjectileSpell::SpawnArcProjectile(const FVector& TargetLocation, float OverrideGravityZ)
{
	SpawnMode = EF1ProjectileSpawnMode::Arc;
	CachedTargetLocation = TargetLocation;
	CachedGravityZ = OverrideGravityZ;

	// "땅 찍어도 됨"
	StartTargetDataTask(EF1TargetingType::Ground);
}

// ============================================================================
// [2단계] 서버: 데이터 수신 및 캐싱 (쏘지 않음!)
// ============================================================================
void UF1ProjectileSpell::OnClientDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	if (!GetOwningActorFromActorInfo()->HasAuthority()) return;

	// 1. 데이터에서 HitResult 확인
	const FGameplayAbilityTargetData_SingleTargetHit* HitData = static_cast<const FGameplayAbilityTargetData_SingleTargetHit*>(Data.Get(0));

	// (만약 SingleTargetHit이 아니라면 LocationInfo로 시도)
	const FGameplayAbilityTargetData_LocationInfo* LocationData = static_cast<const FGameplayAbilityTargetData_LocationInfo*>(Data.Get(0));

	// 초기화
	CachedTargetLocation = FVector::ZeroVector;
	CachedHomingTarget = nullptr;

	// A. 만약 HitResult(마우스 커서 충돌) 데이터가 있다면?
	if (HitData && HitData->HitResult.IsValidBlockingHit())
	{
		// 위치 저장
		CachedTargetLocation = HitData->HitResult.Location;

		// [중요] 적을 클릭했다면 HomingTarget으로 저장!
		AActor* HitActor = HitData->HitResult.GetActor();
		if (HitActor && HitActor->Implements<UF1CombatInterface>()) // 적군인지 확인 (인터페이스나 태그로)
		{
			CachedHomingTarget = HitActor;
			SpawnMode = EF1ProjectileSpawnMode::Homing; // 자동으로 유도 모드 전환
			UE_LOG(LogTemp, Warning, TEXT("[Ability] Target Found: %s (Homing Mode)"), *HitActor->GetName());
		}
		else
		{
			SpawnMode = EF1ProjectileSpawnMode::Linear; // 땅 클릭이면 직사
		}
	}
	// B. 그냥 위치 데이터만 있다면?
	else if (LocationData)
	{
		CachedTargetLocation = LocationData->TargetLocation.LiteralTransform.GetLocation();
		SpawnMode = EF1ProjectileSpawnMode::Linear;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Ability] Data Cached. Waiting for Notify..."));
}

// ============================================================================
// [3단계] 서버: 실제 발사 (Anim Notify에서 호출)
// ============================================================================
void UF1ProjectileSpell::SpawnProjectileExecution()
{
	// [디버그 1] 함수 호출 및 권한 확인
	const bool bIsServer = GetOwningActorFromActorInfo()->HasAuthority();
	UE_LOG(LogTemp, Warning, TEXT("[DEBUG] SpawnProjectileExecution Called. Authority: %d"), bIsServer);

	if (!bIsServer) return;

	// [중요 체크 1] 프로젝타일 클래스가 비어있는지 확인
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[DEBUG] FAILED: ProjectileClass is NULL! check GA Blueprint Defaults."));
		// 클래스가 없으면 더 진행하지 않고 종료 (스킬 쿨타임만 돌게 됨)
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	// [디버그 2] 모드 및 타겟 데이터 확인
	UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Spawn Mode: %d (0:Linear, 1:Arc, 2:Homing)"), (int32)SpawnMode);
	UE_LOG(LogTemp, Warning, TEXT("[Ability] Executing Spawn at Cached Location: %s"), *CachedTargetLocation.ToString());

	// [디버그 3] 소켓 위치 확인
	FVector SocketLocation = FVector::ZeroVector;
	if (IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo()))
	{
		SocketLocation = CombatInterface->GetCombatSocketLocation();
	}

	// 소켓 위치가 0,0,0이면 보통 문제가 있는 것임
	if (SocketLocation.IsZero())
	{
		UE_LOG(LogTemp, Error, TEXT("[DEBUG] WARNING: SocketLocation is Zero! Interface implementation missing?"));
		// 비상시 액터 위치라도 사용하도록 보정
		SocketLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Socket Location: %s"), *SocketLocation.ToString());
	}

	// [시각적 디버깅] 서버 기준 발사 위치에 빨간 구체를 그립니다 (5초간 유지)
	DrawDebugSphere(GetWorld(), SocketLocation, 20.0f, 12, FColor::Red, false, 5.0f);


	FTransform SpawnTransform;
	AF1Projectile* Projectile = nullptr;

	// -------------------------------------------------------------------------
	// A. Homing Mode
	// -------------------------------------------------------------------------
	if (SpawnMode == EF1ProjectileSpawnMode::Homing)
	{
		if (CachedHomingTarget)
		{
			FRotator Rotation = (CachedHomingTarget->GetActorLocation() - SocketLocation).Rotation();
			Rotation.Pitch = 0.f;
			SpawnTransform.SetLocation(SocketLocation);
			SpawnTransform.SetRotation(Rotation.Quaternion());

			Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
				ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
				Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			if (Projectile)
			{
				Projectile->SetHomingTarget(CachedHomingTarget);
				FinishSpawningProjectile(Projectile, CachedHomingTarget->GetActorLocation());
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[DEBUG] FAILED: Homing Mode but No CachedHomingTarget!"));
		}
	}
	// -------------------------------------------------------------------------
	// B. Arc Mode (작성자님이 사용 중인 모드)
	// -------------------------------------------------------------------------
	else if (SpawnMode == EF1ProjectileSpawnMode::Arc)
	{
		const float MaxRange = 1500.0f;
		float TossSpeed = 1500.0f;
		FVector OutLaunchVelocity = FVector::ZeroVector;
		FVector AdjustedTarget = CachedTargetLocation;
		FVector ToTarget = CachedTargetLocation - SocketLocation;

		// 사거리 보정 (높이차 무시하고 2D 거리로만 제한)
		if (ToTarget.Size2D() > MaxRange)
		{
			AdjustedTarget = SocketLocation + (ToTarget.GetSafeNormal2D() * MaxRange);
			AdjustedTarget.Z = CachedTargetLocation.Z; // 높이는 원래 타겟 높이 유지
		}

		// 중력 보정 로직
		float EffectiveGravityZ = CachedGravityZ;
		if (FMath::IsNearlyZero(EffectiveGravityZ))
		{
			if (AF1Projectile* ProjectileCDO = ProjectileClass->GetDefaultObject<AF1Projectile>())
			{
				if (ProjectileCDO->ProjectileMovement)
				{
					EffectiveGravityZ = GetWorld()->GetGravityZ() * ProjectileCDO->ProjectileMovement->ProjectileGravityScale;
				}
			}
		}

		// [핵심] 곡사 해법 찾기
		bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity(
			this, OutLaunchVelocity, SocketLocation, AdjustedTarget, TossSpeed,
			false, 0.f, EffectiveGravityZ, ESuggestProjVelocityTraceOption::DoNotTrace);

		if (bHaveSolution)
		{
			UE_LOG(LogTemp, Log, TEXT("[DEBUG] Arc Solution Found. Velocity: %s"), *OutLaunchVelocity.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[DEBUG] No Arc Solution! Using Linear Fallback."));
			FVector Direction = (AdjustedTarget - SocketLocation).GetSafeNormal();
			OutLaunchVelocity = Direction * TossSpeed;
		}

		SpawnTransform.SetLocation(SocketLocation);
		// 속도 방향대로 회전시킴 (곡사니까 위를 볼 수 있음)
		SpawnTransform.SetRotation(OutLaunchVelocity.Rotation().Quaternion());

		// 스폰 시도
		Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
			ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (Projectile)
		{
			if (Projectile->ProjectileMovement)
			{
				Projectile->ProjectileMovement->Velocity = OutLaunchVelocity;
				Projectile->ProjectileMovement->UpdateComponentVelocity(); // 속도 즉시 적용
			}
			FinishSpawningProjectile(Projectile, AdjustedTarget);
			UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Arc Projectile Spawned SUCCESS!"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[DEBUG] SpawnActorDeferred FAILED! Check Physics/Collision settings."));
		}
	}
	// -------------------------------------------------------------------------
	// C. Linear Mode
	// -------------------------------------------------------------------------
	else
	{
		FRotator Rotation = (CachedTargetLocation - SocketLocation).Rotation();
		Rotation.Pitch = 0.f;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
			ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (Projectile)
		{
			FinishSpawningProjectile(Projectile, CachedTargetLocation);
		}
	}

	// 어빌리티 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UF1ProjectileSpell::FinishSpawningProjectile(AF1Projectile* Projectile, const FVector& HitLocation)
{
	// 기존과 동일한 공통 로직
	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	Projectile->SetOwner(OwnerActor);

	if (OwnerActor)
	{
		UPrimitiveComponent* ProjRoot = Cast<UPrimitiveComponent>(Projectile->GetRootComponent());
		UPrimitiveComponent* OwnerRoot = Cast<UPrimitiveComponent>(OwnerActor->GetRootComponent());
		if (ProjRoot && OwnerRoot)
		{
			ProjRoot->IgnoreActorWhenMoving(OwnerActor, true);
			OwnerRoot->IgnoreActorWhenMoving(Projectile, true);
		}
	}

	APawn* AvatarPawn = Cast<APawn>(OwnerActor);
	AActor* RealInstigator = (AvatarPawn && AvatarPawn->GetPlayerState()) ? AvatarPawn->GetPlayerState() : OwnerActor;

	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
	FGameplayEffectContextHandle ContextHandle = SourceASC->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddInstigator(RealInstigator, Projectile);
	ContextHandle.AddSourceObject(Projectile);

	FHitResult HitResult;
	HitResult.Location = HitLocation;
	ContextHandle.AddHitResult(HitResult);

	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), ContextHandle);
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamage);

	Projectile->DamageEffectSpecHandle = SpecHandle;
	Projectile->FinishSpawning(Projectile->GetTransform());
}