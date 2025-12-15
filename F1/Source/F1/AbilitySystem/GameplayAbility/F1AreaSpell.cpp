#include "AbilitySystem/GameplayAbility/F1AreaSpell.h"
#include "Actor/F1MeteorCircle.h"
#include "Interaction/F1CombatInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/F1GameplayTags.h"
#include "DrawDebugHelpers.h"

// ============================================================================
// 1. 진입점 (Blueprint Callable)
// ============================================================================
void UF1AreaSpell::SpawnAreaActor(const FVector& TargetLocation)
{
	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	if (!AvatarPawn) return;

	// [DEBUG] 호출 로그 확인
	UE_LOG(LogTemp, Warning, TEXT("[SpawnDebug] 1. SpawnAreaActor Called. Actor: %s, Role: %d"), *GetNameSafe(AvatarPawn), (int32)AvatarPawn->GetLocalRole());

	// =================================================================
	// [핵심 수정] "직접 조종하는 클라이언트(혹은 호스트)"만 스폰 요청을 보냄
	// =================================================================
	if (AvatarPawn->IsLocallyControlled())
	{
		// A. 내가 서버(Host)라면? -> 바로 실행
		if (AvatarPawn->HasAuthority())
		{
			ServerSpawnAreaActor_Implementation(TargetLocation);
		}
		// B. 내가 클라이언트라면? -> 서버에게 "해줘"라고 RPC 요청
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[SpawnDebug] Client detected. Requesting Server RPC..."));
			ServerSpawnAreaActor(TargetLocation);
		}
	}
}

// ============================================================================
// 2. Server RPC 유효성 검사
// ============================================================================
bool UF1AreaSpell::ServerSpawnAreaActor_Validate(const FVector& TargetLocation)
{
	// 보안 검사가 필요하다면 여기서 수행 (예: 쿨타임 조작 확인 등)
	// 지금은 무조건 통과
	return true;
}

// ============================================================================
// 3. 실제 스폰 로직 (Server Only)
// ============================================================================
void UF1AreaSpell::ServerSpawnAreaActor_Implementation(const FVector& TargetLocation)
{
	// 여기는 무조건 서버입니다. 안심하고 스폰 로직을 수행합니다.
	AActor* Avatar = GetAvatarActorFromActorInfo();

	UE_LOG(LogTemp, Warning, TEXT("[SpawnDebug] 2. Server Implementation Started. Actor: %s"), *GetNameSafe(Avatar));

	// [DEBUG] 클래스 유효성 검사
	if (!AreaActorClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[SpawnDebug] CRITICAL FAILED: AreaActorClass is NULL! Check BP_AreaSpell Class Defaults."));
		return;
	}

	IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(Avatar);
	if (!CombatInterface) return;

	const FVector SocketLoc = CombatInterface->GetCombatSocketLocation();
	float MaxRange = 1000.0f; // TODO: AttributeSet에서 가져오도록 수정 권장

	// ---------------------------------------------------------
	// 1. 사거리 계산 및 보정 (Clamp Logic)
	// ---------------------------------------------------------
	FVector Direction = (TargetLocation - SocketLoc).GetSafeNormal();
	float Distance = FVector::Dist(SocketLoc, TargetLocation);

	FVector SpawnLoc = TargetLocation;

	if (Distance > MaxRange)
	{
		SpawnLoc = SocketLoc + (Direction * MaxRange);
		// [DEBUG] 사거리 제한 걸림 (노란선)
		DrawDebugLine(GetWorld(), SocketLoc, SpawnLoc, FColor::Yellow, false, 3.0f, 0, 2.0f);
	}
	else
	{
		// [DEBUG] 정상 사거리 (녹색선)
		DrawDebugLine(GetWorld(), SocketLoc, SpawnLoc, FColor::Green, false, 3.0f, 0, 2.0f);
	}

	// ---------------------------------------------------------
	// 2. 바닥 높이 찾기 (LineTrace)
	// ---------------------------------------------------------
	FHitResult HitResult;
	FVector TraceStart = SpawnLoc;
	TraceStart.Z += 500.0f;
	FVector TraceEnd = SpawnLoc;
	TraceEnd.Z -= 500.0f;

	// [DEBUG] 트레이스 라인 (파란색)
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Blue, false, 5.0f, 0, 1.0f);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility // 지형 채널
	);

	if (bHit)
	{
		SpawnLoc = HitResult.Location;
		// [DEBUG] 바닥 찾음 (녹색 박스)
		DrawDebugBox(GetWorld(), SpawnLoc, FVector(10.f), FColor::Green, false, 5.0f);
	}
	else
	{
		// [DEBUG] 바닥 못 찾음 (빨간 박스)
		DrawDebugBox(GetWorld(), SpawnLoc, FVector(10.f), FColor::Red, false, 5.0f);
		UE_LOG(LogTemp, Warning, TEXT("[SpawnDebug] WARNING: Floor Trace Failed! Spawning at original Z."));
	}

	// ---------------------------------------------------------
	// 3. 장판 소환 (Deferred Spawn)
	// ---------------------------------------------------------
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLoc);
	// 필요시 Rotation 설정: SpawnTransform.SetRotation(FRotator(0.f, FMath::RandRange(0.f, 360.f), 0.f).Quaternion());

	UE_LOG(LogTemp, Log, TEXT("[SpawnDebug] Attempting to spawn class: %s at %s"), *GetNameSafe(AreaActorClass), *SpawnLoc.ToString());

	AF1MeteorCircle* AreaActor = GetWorld()->SpawnActorDeferred<AF1MeteorCircle>(
		AreaActorClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (AreaActor)
	{
		// ---------------------------------------------------------
		// 4. 데미지 정보(Spec) 전달
		// ---------------------------------------------------------
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(AreaActor);

		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		// [DEBUG] Spec 유효성 검사
		if (!SpecHandle.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("[SpawnDebug] Error: SpecHandle is Invalid! Check DamageEffectClass in BP."));
		}

		// 데미지 수치 전달 (SetByCaller)
		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamage);

		AreaActor->DamageEffectSpecHandle = SpecHandle;

		// ---------------------------------------------------------
		// 5. 소환 완료 (Finish Spawning)
		// ---------------------------------------------------------
		AreaActor->FinishSpawning(SpawnTransform);

		// [DEBUG] 최종 성공 (청록색 구체)
		DrawDebugSphere(GetWorld(), SpawnLoc, 50.0f, 12, FColor::Cyan, false, 3.0f);
		UE_LOG(LogTemp, Log, TEXT("[SpawnDebug] SUCCESS: Actor Spawned on SERVER: %s"), *GetNameSafe(AreaActor));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SpawnDebug] FATAL: SpawnActorDeferred returned NULL!"));
	}
}