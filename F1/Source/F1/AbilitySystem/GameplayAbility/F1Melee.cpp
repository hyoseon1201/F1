
#include "AbilitySystem/GameplayAbility/F1Melee.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/F1GameplayTags.h"
#include "Engine/OverlapResult.h"
#include "DrawDebugHelpers.h"

void UF1Melee::TriggerMeleeAttack()
{
	// 1. 내 캐릭터(Avatar) 확인
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	// 2. 주변 적 탐지 (Sphere Overlap)
	TArray<FOverlapResult> OverlapResults;
	FVector Origin = AvatarActor->GetActorLocation();

	if (bDrawDebug)
	{
		// 파라미터: 월드, 중심점, 반지름, 분할수(원 부드러움), 색상, 영구지속여부, 지속시간
		DrawDebugSphere(GetWorld(), Origin, AttackRadius, 12, FColor::Red, false, 2.0f);
	}

	// 내 위치를 중심으로 구체 검사
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor); // 나는 때리면 안 됨

	// 몬스터가 플레이어를 때려야 하므로, 보통 Pawn 채널을 검사합니다.
	// (프로젝트 설정에 따라 'Player' 전용 채널을 쓰셔도 됩니다.)
	bool bHit = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		Origin,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn), // 폰(캐릭터)만 검사
		FCollisionShape::MakeSphere(AttackRadius),
		QueryParams
	);

	if (bHit)
	{
		// 3. 감지된 적들에게 데미지 적용
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* TargetActor = Result.GetActor();

			if (bDrawDebug)
			{
				DrawDebugPoint(GetWorld(), TargetActor->GetActorLocation(), 10.0f, FColor::Green, false, 2.0f);
			}

			// 적군인지 확인 (팀 ID 체크 또는 태그 체크 등)
			// 여기서는 간단하게 ICombatInterface의 GetTeam() 등을 활용하거나
			// GAS의 FriendlyFire 옵션을 통해 Effect 적용 시 걸러낼 수 있습니다.
			if (TargetActor && TargetActor != AvatarActor)
			{
				// [부모 클래스 활용] DamageEffectClass 적용
				if (DamageEffectClass)
				{
					FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
					ContextHandle.AddSourceObject(AvatarActor);

					// Spec 생성 (레벨, 데미지 값 등 설정)
					const float AbilityLevel = GetAbilityLevel();
					FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(DamageEffectClass, AbilityLevel, ContextHandle);

					// 데미지 값(SetByCaller) 설정 (부모 클래스의 변수 사용)
					// F1GameplayTags::Get().Damage_Fire 등의 태그로 값을 넘겨줌
					if (SpecHandle.IsValid())
					{
						// ScalableFloat 값을 현재 레벨에 맞춰 계산
						float ScaledDamage = Damage.GetValueAtLevel(AbilityLevel);
						UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaledDamage);

						// 효과 적용!
						GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
					}
				}
			}
		}
	}
}