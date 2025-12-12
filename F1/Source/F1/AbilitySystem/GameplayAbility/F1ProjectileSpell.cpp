// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/GameplayAbility/F1ProjectileSpell.h"
#include "AbilitySystem/Actor/F1Projectile.h"
#include "Interaction/F1CombatInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/F1GameplayTags.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PrimitiveComponent.h"

void UF1ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

// ============================================================================
// 1. 직사 발사 (Linear)
// ============================================================================
void UF1ProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

	// 목표 지점을 바라보는 회전값 계산 (Pitch 0으로 수평 유지)
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	Rotation.Pitch = 0.f;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	AF1Projectile* Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Projectile)
	{
		// 주인 설정 및 자폭 방지 (필수)
		AActor* OwnerActor = GetAvatarActorFromActorInfo();
		Projectile->SetOwner(OwnerActor);

		if (OwnerActor)
		{
			UPrimitiveComponent* ProjectileRoot = Cast<UPrimitiveComponent>(Projectile->GetRootComponent());
			UPrimitiveComponent* OwnerRoot = Cast<UPrimitiveComponent>(OwnerActor->GetRootComponent());
			if (ProjectileRoot && OwnerRoot)
			{
				ProjectileRoot->IgnoreActorWhenMoving(OwnerActor, true);
				OwnerRoot->IgnoreActorWhenMoving(Projectile, true);
			}
		}

		// GAS 데미지 설정
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);

		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);

		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);

		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());

		// 태그: DamageType.Physical 등 프로젝트에 맞는 태그 사용
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FF1GameplayTags::Get().DamageType_Physical, ScaledDamage);

		Projectile->DamageEffectSpecHandle = SpecHandle;

		// 발사
		Projectile->FinishSpawning(SpawnTransform);
	}
}

// ============================================================================
// 2. 유도 발사 (Homing)
// ============================================================================
void UF1ProjectileSpell::SpawnHomingProjectile(AActor* HomingTarget)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority() || !HomingTarget) return;

	IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

	// 타겟을 바라보며 시작
	FRotator Rotation = (HomingTarget->GetActorLocation() - SocketLocation).Rotation();
	Rotation.Pitch = 0.f;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	AF1Projectile* Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Projectile)
	{
		// 유도 타겟 설정
		Projectile->SetHomingTarget(HomingTarget);

		// 주인 설정 및 자폭 방지
		AActor* OwnerActor = GetAvatarActorFromActorInfo();
		Projectile->SetOwner(OwnerActor);

		if (OwnerActor)
		{
			UPrimitiveComponent* ProjectileRoot = Cast<UPrimitiveComponent>(Projectile->GetRootComponent());
			UPrimitiveComponent* OwnerRoot = Cast<UPrimitiveComponent>(OwnerActor->GetRootComponent());
			if (ProjectileRoot && OwnerRoot)
			{
				ProjectileRoot->IgnoreActorWhenMoving(OwnerActor, true);
				OwnerRoot->IgnoreActorWhenMoving(Projectile, true);
			}
		}

		// GAS 데미지 설정
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);

		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);

		FHitResult HitResult;
		HitResult.Location = HomingTarget->GetActorLocation();
		EffectContextHandle.AddHitResult(HitResult);

		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());

		// 태그 주의 (프로젝트에 맞게 수정)
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FF1GameplayTags::Get().DamageType_Physical, ScaledDamage);

		Projectile->DamageEffectSpecHandle = SpecHandle;

		// 발사
		Projectile->FinishSpawning(SpawnTransform);
	}
}

// ============================================================================
// 3. 곡사 발사 (Arc)
// ============================================================================
void UF1ProjectileSpell::SpawnArcProjectile(const FVector& TargetLocation, float OverrideGravityZ)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

	// [설정] 최대 사거리 및 속도
	const float MaxRange = 1500.0f;
	float TossSpeed = 1500.0f;
	FVector OutLaunchVelocity = FVector::ZeroVector;

	// [로직 1] 사거리 제한 (Range Clamping)
	FVector AdjustedTarget = TargetLocation;
	FVector ToTarget = TargetLocation - SocketLocation;

	if (ToTarget.Size2D() > MaxRange)
	{
		AdjustedTarget = SocketLocation + (ToTarget.GetSafeNormal2D() * MaxRange);
		AdjustedTarget.Z = TargetLocation.Z; // 높이 유지
	}

	// [로직 2] 중력 보정 (BP GravityScale 반영)
	float EffectiveGravityZ = OverrideGravityZ;
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

	// [로직 3] 탄도학 계산
	bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity(
		this,
		OutLaunchVelocity,
		SocketLocation,
		AdjustedTarget,
		TossSpeed,
		false,  // Low Arc (던지기 궤적)
		0.0f,   // CollisionRadius
		EffectiveGravityZ,
		ESuggestProjVelocityTraceOption::DoNotTrace,
		FCollisionResponseParams::DefaultResponseParam,
		TArray<AActor*>(),
		true    // DrawDebug
	);

	// 계산 실패 시 직사 폴백
	if (!bHaveSolution)
	{
		FVector Direction = (AdjustedTarget - SocketLocation).GetSafeNormal();
		OutLaunchVelocity = Direction * TossSpeed;
		UE_LOG(LogTemp, Warning, TEXT("[ArcShot] FAILED! Launching Linear."));
	}

	// [핵심] 속도 방향으로 초기 회전 설정
	FRotator Rotation = OutLaunchVelocity.Rotation();
	FTransform SpawnTransform(Rotation, SocketLocation);

	AF1Projectile* Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Projectile)
	{
		// 주인 설정 및 자폭 방지
		AActor* OwnerActor = GetAvatarActorFromActorInfo();
		Projectile->SetOwner(OwnerActor);

		if (OwnerActor)
		{
			UPrimitiveComponent* ProjectileRoot = Cast<UPrimitiveComponent>(Projectile->GetRootComponent());
			UPrimitiveComponent* OwnerRoot = Cast<UPrimitiveComponent>(OwnerActor->GetRootComponent());

			if (ProjectileRoot && OwnerRoot)
			{
				ProjectileRoot->IgnoreActorWhenMoving(OwnerActor, true);
				OwnerRoot->IgnoreActorWhenMoving(Projectile, true);
			}
		}

		// [핵심] 계산된 속도 주입
		if (Projectile->ProjectileMovement)
		{
			Projectile->ProjectileMovement->Velocity = OutLaunchVelocity;
			Projectile->ProjectileMovement->UpdateComponentVelocity();
		}

		// GAS 데미지 설정
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);

		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);

		FHitResult HitResult;
		HitResult.Location = AdjustedTarget;
		EffectContextHandle.AddHitResult(HitResult);

		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());

		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FF1GameplayTags::Get().DamageType_Physical, ScaledDamage);

		Projectile->DamageEffectSpecHandle = SpecHandle;

		// 발사
		Projectile->FinishSpawning(SpawnTransform);
	}
}