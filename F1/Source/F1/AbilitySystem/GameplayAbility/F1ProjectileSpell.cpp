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
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "DrawDebugHelpers.h"

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

	if (Projectile)
	{
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
}

void UF1ProjectileSpell::SpawnHomingProjectile(AActor* HomingTarget)
{
	// 1. 서버 권한 및 타겟 확인
	if (!GetAvatarActorFromActorInfo()->HasAuthority() || !HomingTarget) return;

	IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

	// 2. 회전값 설정
	FRotator Rotation = (HomingTarget->GetActorLocation() - SocketLocation).Rotation();
	Rotation.Pitch = 0.f;

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());

	// 3. 지연 스폰
	AF1Projectile* Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	if (Projectile)
	{
		// 4. 유도 타겟 설정
		Projectile->SetHomingTarget(HomingTarget);

		AActor* OwnerActor = GetAvatarActorFromActorInfo();
		Projectile->SetOwner(OwnerActor);

		// 5. 자폭 방지
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
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamage);

		Projectile->DamageEffectSpecHandle = SpecHandle;
		Projectile->FinishSpawning(SpawnTransform);
	}
}

void UF1ProjectileSpell::SpawnArcProjectile(const FVector& TargetLocation, float OverrideGravityZ)
{
    if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

    IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo());
    if (!CombatInterface) return;

    const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();

    // ================================================================
    // [디버깅 1] 입력값 확인 (시작점: 초록 / 목표점: 빨강)
    // ================================================================
    DrawDebugSphere(GetWorld(), SocketLocation, 30.0f, 12, FColor::Green, false, 5.0f);
    DrawDebugSphere(GetWorld(), TargetLocation, 30.0f, 12, FColor::Red, false, 5.0f);
    DrawDebugLine(GetWorld(), SocketLocation, TargetLocation, FColor::White, false, 5.0f, 0, 2.0f);

    // ----------------------------------------------------------------
    // 변수 설정
    // ----------------------------------------------------------------
    const float MaxRange = 800.0f;
    float TossSpeed = 1500.0f; // [체크] 이 값이 너무 낮으면 닿지 않습니다.
    FVector OutLaunchVelocity = FVector::ZeroVector;

    // ================================================================
    // [로직 1] 사거리 제한 및 높이 보정 (핵심 수정)
    // ================================================================
    FVector AdjustedTarget = TargetLocation;
    FVector ToTarget = TargetLocation - SocketLocation;

    // 1. 거리가 MaxRange보다 멀면 당겨옴
    if (ToTarget.Size2D() > MaxRange)
    {
        // X, Y 좌표를 사거리 끝으로 당김
        FVector2D ClampedXY = FVector2D(SocketLocation) + (FVector2D(ToTarget).GetSafeNormal() * MaxRange);
        AdjustedTarget.X = ClampedXY.X;
        AdjustedTarget.Y = ClampedXY.Y;

        // 2. [추가] 당겨온 위치의 "바닥 높이(Z)"를 다시 찾음 (레이캐스트)
        // 하늘 높은 곳이나 땅속을 조준하지 않도록, 위에서 아래로 레이저를 쏴서 바닥을 찾습니다.
        FHitResult FloorHit;
        FVector TraceStart = FVector(AdjustedTarget.X, AdjustedTarget.Y, SocketLocation.Z + 1000.0f); // 높은 곳에서
        FVector TraceEnd = FVector(AdjustedTarget.X, AdjustedTarget.Y, SocketLocation.Z - 2000.0f);   // 아래로

        // WorldStatic(지형)만 찾습니다.
        bool bHitFloor = GetWorld()->LineTraceSingleByChannel(
            FloorHit,
            TraceStart,
            TraceEnd,
            ECC_WorldStatic
        );

        if (bHitFloor)
        {
            // 바닥을 찾았으면 그 높이로 타겟 수정
            AdjustedTarget.Z = FloorHit.Location.Z;
        }
        else
        {
            // 바닥이 없으면(절벽 밖 등) 그냥 발사 높이랑 같게 맞춤 (수평 발사)
            AdjustedTarget.Z = SocketLocation.Z;
        }

        // [디버그] 파란 공 위치가 이제 바닥에 딱 붙어있어야 합니다.
        DrawDebugSphere(GetWorld(), AdjustedTarget, 25.0f, 12, FColor::Blue, false, 5.0f);
    }

    // 중력 보정 로직
    float EffectiveGravityZ = OverrideGravityZ;
    if (FMath::IsNearlyZero(EffectiveGravityZ))
    {
        if (AF1Projectile* CDO = ProjectileClass->GetDefaultObject<AF1Projectile>())
        {
            if (CDO->ProjectileMovement)
            {
                EffectiveGravityZ = GetWorld()->GetGravityZ() * CDO->ProjectileMovement->ProjectileGravityScale;
            }
        }
    }

    // ----------------------------------------------------------------
    // 탄도학 계산
    // ----------------------------------------------------------------
    bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity(
        this,
        OutLaunchVelocity,
        SocketLocation,
        AdjustedTarget,
        TossSpeed,
        false, // HighArc: False (중요)
        0.0f,
        EffectiveGravityZ,
        ESuggestProjVelocityTraceOption::DoNotTrace,
        FCollisionResponseParams::DefaultResponseParam,
        TArray<AActor*>(),
        true // [디버그 3] 엔진 자체 궤적 그리기 (초록색 선)
    );

    if (!bHaveSolution)
    {
        // 실패 시 노란색 화살표 (직사)
        FVector Direction = (AdjustedTarget - SocketLocation).GetSafeNormal();
        OutLaunchVelocity = Direction * TossSpeed;
        DrawDebugDirectionalArrow(GetWorld(), SocketLocation, SocketLocation + OutLaunchVelocity, 500.0f, FColor::Yellow, false, 5.0f, 0, 5.0f);
        UE_LOG(LogTemp, Error, TEXT("FAILED! TossSpeed(%f) is too low for Distance(%f)"), TossSpeed, FVector::Dist(SocketLocation, AdjustedTarget));
    }
    else
    {
        // 성공 시 보라색 화살표 (계산된 발사 벡터)
        DrawDebugDirectionalArrow(GetWorld(), SocketLocation, SocketLocation + OutLaunchVelocity, 500.0f, FColor::Purple, false, 5.0f, 0, 5.0f);
        UE_LOG(LogTemp, Warning, TEXT("SUCCESS! Velocity: %s"), *OutLaunchVelocity.ToString());
    }

    // ----------------------------------------------------------------
    // 스폰 및 발사
    // ----------------------------------------------------------------
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
		AActor* OwnerActor = GetAvatarActorFromActorInfo();
		Projectile->SetOwner(OwnerActor);

		// 1. 자폭 방지
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

		// 2. 속도 주입
		if (Projectile->ProjectileMovement)
		{
			Projectile->ProjectileMovement->Velocity = OutLaunchVelocity;
		}

		// 3. GAS 데미지 설정 (핸들 생성 부분 포함)
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());

		// [여기서 정의해야 합니다!]
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);

		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);

		FHitResult HitResult;
		HitResult.Location = AdjustedTarget; // 보정된 위치 사용
		EffectContextHandle.AddHitResult(HitResult);

		// [디버그] 클래스 설정 확인
		if (!DamageEffectClass)
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnArcProjectile: DamageEffectClass is NULL! Check BP Defaults."));
		}

		// 이제 EffectContextHandle을 사용할 수 있습니다.
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		// [디버그] 핸들 생성 확인
		if (SpecHandle.Data.IsValid())
		{
			// 데미지 값 적용
			const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
			UE_LOG(LogTemp, Warning, TEXT("Applying Damage: %f"), ScaledDamage);
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamage);

			Projectile->DamageEffectSpecHandle = SpecHandle;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnArcProjectile: SpecHandle is INVALID!"));
		}

		// 4. 스폰 완료
		Projectile->FinishSpawning(SpawnTransform);
	}
}