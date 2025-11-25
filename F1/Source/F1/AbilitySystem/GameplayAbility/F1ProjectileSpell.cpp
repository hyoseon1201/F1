// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/GameplayAbility/F1ProjectileSpell.h"
#include "Interaction/F1CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystem/Actor/F1Projectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/F1GameplayTags.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

void UF1ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UF1ProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	Rotation.Pitch = 0.f;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	AF1Projectile* Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
		ProjectileClass,
		SpawnTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	Projectile->SetOwner(GetAvatarActorFromActorInfo());
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

	FF1GameplayTags GameplayTags = FF1GameplayTags::Get();

	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamage);

	Projectile->DamageEffectSpecHandle = SpecHandle;
	Projectile->FinishSpawning(SpawnTransform);
}

void UF1ProjectileSpell::SpawnHomingProjectile(AActor* HomingTarget)
{
	// 1. 서버 권한 및 타겟 확인
	if (!GetAvatarActorFromActorInfo()->HasAuthority() || !HomingTarget) return;

	IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	// 2. 발사 위치 및 회전 계산
	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

	// 초기 발사 방향은 타겟을 바라보게 설정
	FRotator Rotation = (HomingTarget->GetActorLocation() - SocketLocation).Rotation();
	Rotation.Pitch = 0.f;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	// 3. 투사체 지연 스폰 (Deferred Spawn)
	AF1Projectile* Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	// 4. [핵심] 유도 타겟 설정 (아까 만든 함수 호출)
	if (Projectile)
	{
		// 투사체에게 "너는 쟤를 쫓아가라"고 명령
		Projectile->SetHomingTarget(HomingTarget);

		// 기존 데미지 설정 로직 (그대로 유지)
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);

		// EffectContext에 타겟 정보 저장
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);

		FHitResult HitResult;
		HitResult.Location = HomingTarget->GetActorLocation(); // 타겟 위치 저장
		EffectContextHandle.AddHitResult(HitResult);

		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		// 데미지 값 적용
		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamage);

		Projectile->DamageEffectSpecHandle = SpecHandle;

		// 5. 스폰 완료
		Projectile->FinishSpawning(SpawnTransform);
	}
}

void UF1ProjectileSpell::SpawnArcProjectile(const FVector& TargetLocation, float OverrideGravityZ)
{
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

	// 1. 발사 속도 벡터 계산
	FVector OutLaunchVelocity = FVector::ZeroVector;
	float TossSpeed = 1000.0f; // 던지는 힘 (투사체 BP의 Speed보다 우선됨)

	// 언리얼이 제공하는 탄도학 계산 함수
	bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity(
		this,
		OutLaunchVelocity,
		SocketLocation,
		TargetLocation,
		TossSpeed,
		false, // HighArc: false면 직사에 가까운 곡사, true면 높이 솟았다 떨어짐
		0.0f,  // CollisionRadius
		OverrideGravityZ,
		ESuggestProjVelocityTraceOption::DoNotTrace // 트레이스 없이 수학적으로만 계산
	);

	if (bHaveSolution)
	{
		// 2. 계산된 방향으로 회전값 설정
		FRotator Rotation = OutLaunchVelocity.Rotation();
		FTransform SpawnTransform(Rotation, SocketLocation);

		// 3. 지연 스폰
		AF1Projectile* Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (Projectile)
		{
			// [핵심] 계산된 속도를 투사체 무브먼트에 주입!
			// 이렇게 해야 ProjectileMovement가 계산된 궤적대로 날아갑니다.
			if (Projectile->ProjectileMovement)
			{
				Projectile->ProjectileMovement->Velocity = OutLaunchVelocity;
			}

			// --- 기존 데미지 설정 로직 (복붙) ---
			const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
			FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
			EffectContextHandle.SetAbility(this);
			EffectContextHandle.AddSourceObject(Projectile);
			TArray<TWeakObjectPtr<AActor>> Actors;
			Actors.Add(Projectile);
			EffectContextHandle.AddActors(Actors);
			FHitResult HitResult;
			HitResult.Location = TargetLocation;
			EffectContextHandle.AddHitResult(HitResult);

			const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
			const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamage);
			Projectile->DamageEffectSpecHandle = SpecHandle;
			// ------------------------------------

			Projectile->FinishSpawning(SpawnTransform);
		}
	}
}
