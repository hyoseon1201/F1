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
	SpawnMode = EProjectileSpawnMode::Linear;
	CachedTargetLocation = ProjectileTargetLocation;

	// "나는 땅을 찍어도 발사할 거야"
	StartTargetDataTask(EF1TargetingType::Ground);
}

// 2. 유도 발사 (타겟팅 -> Enemy 모드)
void UF1ProjectileSpell::SpawnHomingProjectile(AActor* HomingTarget)
{
	SpawnMode = EProjectileSpawnMode::Homing;
	CachedHomingTarget = HomingTarget;

	// "나는 적이 아니면 발사 안 해"
	StartTargetDataTask(EF1TargetingType::Enemy);
}

// 3. 곡사 발사 (보통 논타겟 -> Ground 모드)
void UF1ProjectileSpell::SpawnArcProjectile(const FVector& TargetLocation, float OverrideGravityZ)
{
	SpawnMode = EProjectileSpawnMode::Arc;
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
			SpawnMode = EProjectileSpawnMode::Homing; // 자동으로 유도 모드 전환
			UE_LOG(LogTemp, Warning, TEXT("[Ability] Target Found: %s (Homing Mode)"), *HitActor->GetName());
		}
		else
		{
			SpawnMode = EProjectileSpawnMode::Linear; // 땅 클릭이면 직사
		}
	}
	// B. 그냥 위치 데이터만 있다면?
	else if (LocationData)
	{
		CachedTargetLocation = LocationData->TargetLocation.LiteralTransform.GetLocation();
		SpawnMode = EProjectileSpawnMode::Linear;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Ability] Data Cached. Waiting for Notify..."));
}

// ============================================================================
// [3단계] 서버: 실제 발사 (Anim Notify에서 호출)
// ============================================================================
void UF1ProjectileSpell::SpawnProjectileExecution()
{
	// 서버만 실행
	if (!GetOwningActorFromActorInfo()->HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("[Ability] Executing Spawn at Cached Location: %s"), *CachedTargetLocation.ToString());

	FVector SocketLocation = FVector::ZeroVector;
	if (IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo()))
	{
		SocketLocation = CombatInterface->GetCombatSocketLocation();
	}

	FTransform SpawnTransform;
	AF1Projectile* Projectile = nullptr;

	// -------------------------------------------------------------------------
	// A. Homing Mode
	// -------------------------------------------------------------------------
	if (SpawnMode == EProjectileSpawnMode::Homing)
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
	}
	// -------------------------------------------------------------------------
	// B. Arc Mode
	// -------------------------------------------------------------------------
	else if (SpawnMode == EProjectileSpawnMode::Arc)
	{
		// 곡사 계산 로직
		const float MaxRange = 1500.0f;
		float TossSpeed = 1500.0f;
		FVector OutLaunchVelocity = FVector::ZeroVector;
		FVector AdjustedTarget = CachedTargetLocation; // 캐시된 타겟 사용
		FVector ToTarget = CachedTargetLocation - SocketLocation;

		if (ToTarget.Size2D() > MaxRange)
		{
			AdjustedTarget = SocketLocation + (ToTarget.GetSafeNormal2D() * MaxRange);
			AdjustedTarget.Z = CachedTargetLocation.Z;
		}

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

		bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity(
			this, OutLaunchVelocity, SocketLocation, AdjustedTarget, TossSpeed,
			false, 0.f, EffectiveGravityZ, ESuggestProjVelocityTraceOption::DoNotTrace);

		if (!bHaveSolution)
		{
			FVector Direction = (AdjustedTarget - SocketLocation).GetSafeNormal();
			OutLaunchVelocity = Direction * TossSpeed;
		}

		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(OutLaunchVelocity.Rotation().Quaternion());

		Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
			ProjectileClass, SpawnTransform, GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (Projectile)
		{
			if (Projectile->ProjectileMovement)
			{
				Projectile->ProjectileMovement->Velocity = OutLaunchVelocity;
				Projectile->ProjectileMovement->UpdateComponentVelocity();
			}
			FinishSpawningProjectile(Projectile, AdjustedTarget);
		}
	}
	// -------------------------------------------------------------------------
	// C. Linear Mode (Default)
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

	// [중요] 발사 후 어빌리티 종료
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UF1ProjectileSpell::CacheTargetData(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	// 데이터가 유효한지 확인
	if (DataHandle.Data.IsValidIndex(0))
	{
		// SingleTargetHit(마우스 히트 결과) 형태로 데이터를 꺼냄
		const FGameplayAbilityTargetData_SingleTargetHit* HitData = static_cast<const FGameplayAbilityTargetData_SingleTargetHit*>(DataHandle.Get(0));

		if (HitData && HitData->HitResult.IsValidBlockingHit())
		{
			// 1. 적(Actor) 저장 (유도탄용)
			CachedHomingTarget = HitData->HitResult.GetActor();

			// 2. 위치 저장 (만약 적이 죽거나 사라졌을 때 마지막 위치로 날아가기 위함)
			CachedTargetLocation = HitData->HitResult.Location;

			// 3. 모드 설정 (적이 잡혔으니 Homing)
			SpawnMode = EProjectileSpawnMode::Homing;

			UE_LOG(LogTemp, Warning, TEXT("[Ability] CacheTargetData: Target Cached [%s]"), *GetNameSafe(CachedHomingTarget));
		}
	}
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