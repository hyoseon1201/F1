// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GameplayAbility/F1MeleeSpell.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/F1GameplayTags.h"

void UF1MeleeSpell::ApplyDamageToTarget(AActor* TargetActor)
{
	if (!IsValid(TargetActor)) return;

	const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	if (!SourceASC) return;

	// 1. Context 생성
	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.SetAbility(this);
	EffectContext.AddSourceObject(GetAvatarActorFromActorInfo());
	EffectContext.AddActors({ TargetActor });

	// 히트 위치 정보 추가 (선택사항: 피격 이펙트 등을 위해 넣는 것이 좋음)
	FHitResult HitResult;
	HitResult.Location = TargetActor->GetActorLocation();
	EffectContext.AddHitResult(HitResult);

	// 2. Spec 생성 (GE 클래스 사용)
	const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContext);

	// ======================================================================
	// [추가된 부분] 데미지 타입과 수치 전달 (SetByCaller)
	// ======================================================================
	if (SpecHandle.IsValid())
	{
		// 3. 레벨에 따른 데미지 수치 계산 (CurveTable 등 반영)
		const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());

		// 4. 태그 기반으로 값 전달
		// "이 데미지는 [DamageType] 속성이며, 값은 [ScaledDamage]이다"라고 명시
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			SpecHandle,
			DamageType,     // 헤더에 있는 UPROPERTY 변수 (BP에서 설정: Physical or Magic)
			ScaledDamage    // 계산된 데미지 양
		);
	}
	// ======================================================================

	// 5. 최종 적용
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC)
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}