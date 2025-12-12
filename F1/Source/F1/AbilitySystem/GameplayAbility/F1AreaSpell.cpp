#include "AbilitySystem/GameplayAbility/F1AreaSpell.h"
#include "Actor/F1MeteorCircle.h"
#include "Interaction/F1CombatInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/F1GameplayTags.h"

void UF1AreaSpell::SpawnAreaActor(const FVector& TargetLocation)
{
	// 1. 권한 확인 (서버만 소환 가능)
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const FVector SocketLoc = CombatInterface->GetCombatSocketLocation();
	float MaxRange = 1000.0f; // 나중에 Attribute나 ScalableFloat로 교체 추천

	// 2. 사거리 계산 (Clamp Logic)
	FVector Direction = (TargetLocation - SocketLoc).GetSafeNormal();
	float Distance = FVector::Dist(SocketLoc, TargetLocation);

	FVector SpawnLoc = TargetLocation;

	// 사거리 밖이면 최대 사거리 위치로 당겨옴
	if (Distance > MaxRange)
	{
		SpawnLoc = SocketLoc + (Direction * MaxRange);
	}

	// 3. [중요] 바닥 높이 찾기 (LineTrace)
	// 공중에서 클릭했거나, 언덕 지형일 수 있으므로 정확한 바닥 Z값을 찾습니다.
	FHitResult HitResult;
	FVector TraceStart = SpawnLoc;
	TraceStart.Z += 500.0f; // 위에서
	FVector TraceEnd = SpawnLoc;
	TraceEnd.Z -= 500.0f;   // 아래로 쏘기

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility // 지형(Visibility) 채널 감지
	);

	if (bHit)
	{
		SpawnLoc = HitResult.Location; // 정확한 바닥 위치로 갱신
	}

	// 4. 장판 소환 (Deferred Spawn)
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLoc);
	// 필요하면 회전도 바닥 경사(HitResult.ImpactNormal)에 맞출 수 있음

	AF1MeteorCircle* AreaActor = GetWorld()->SpawnActorDeferred<AF1MeteorCircle>(
		AreaActorClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (AreaActor)
	{
		// 5. 데미지 정보 전달 (드론/투사체와 동일)
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(AreaActor);

		// 데미지 스펙 생성
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		// 데미지 수치 설정 (SetByCaller)
		// 예: R스킬 데미지 공식 적용
		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FF1GameplayTags::Get().DamageType_Physical, ScaledDamage);

		// 장판 액터에게 스펙 전달
		AreaActor->DamageEffectSpecHandle = SpecHandle;

		// 6. 소환 완료
		AreaActor->FinishSpawning(SpawnTransform);
	}
}