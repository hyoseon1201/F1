// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/F1ExCalcBasicDamage.h" // 헤더 파일 변경
#include "AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "GameplayTag/F1GameplayTags.h"
#include "F1AbilityTypes.h"
#include "AbilitySystem/F1AbilitySystemLibrary.h"

// 정적 구조체 이름 변경 (F1PhysicalDamageStatics -> F1BasicDamageStatics)
struct F1BasicDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetrationFlat);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetrationPercent);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalStrikeChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalStrikeDamage);

	F1BasicDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, ArmorPenetrationFlat, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, ArmorPenetrationPercent, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, CriticalStrikeChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, CriticalStrikeDamage, Source, false);
	}
};

static const F1BasicDamageStatics& BasicDamageStatics()
{
	static F1BasicDamageStatics BDStatics;

	return BDStatics;
}

// 클래스 생성자 이름 변경
UF1ExCalcBasicDamage::UF1ExCalcBasicDamage()
{
	RelevantAttributesToCapture.Add(BasicDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(BasicDamageStatics().ArmorPenetrationFlatDef);
	RelevantAttributesToCapture.Add(BasicDamageStatics().ArmorPenetrationPercentDef);
	RelevantAttributesToCapture.Add(BasicDamageStatics().CriticalStrikeChanceDef);
	RelevantAttributesToCapture.Add(BasicDamageStatics().CriticalStrikeDamageDef);
}

// 실행 로직 이식
void UF1ExCalcBasicDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// 기본 물리 피해
	// (물리 피해 태그를 사용하지만, 이제 이 ExecCalc는 "기본 공격 피해"를 담당한다고 가정합니다.)
	float Damage = Spec.GetSetByCallerMagnitude(FF1GameplayTags::Get().DamageType_Physical);

	// 속성 캡처
	float Armor = 0.f;
	float ArmorPenFlat = 0.f;
	float ArmorPenPercent = 0.f;
	float CritChance = 0.f;
	float CritDamage = 0.f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BasicDamageStatics().ArmorDef, EvaluationParameters, Armor);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BasicDamageStatics().ArmorPenetrationFlatDef, EvaluationParameters, ArmorPenFlat);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BasicDamageStatics().ArmorPenetrationPercentDef, EvaluationParameters, ArmorPenPercent);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BasicDamageStatics().CriticalStrikeChanceDef, EvaluationParameters, CritChance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(BasicDamageStatics().CriticalStrikeDamageDef, EvaluationParameters, CritDamage);

	// 방어력 계산
	float ArmorAfterPercent = Armor * (1.f - FMath::Clamp(ArmorPenPercent, 0.f, 1.f));
	float EffectiveArmor = FMath::Max<float>(0.f, ArmorAfterPercent - ArmorPenFlat);
	float DamageMultiplier = 100.f / (100.f + EffectiveArmor);
	float FinalDamage = Damage * DamageMultiplier;

	// 치명타 판정 (유지)
	bool bIsCritical = FMath::FRand() < FMath::Clamp(CritChance, 0.f, 1.f);

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UF1AbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bIsCritical);

	if (bIsCritical)
	{
		FinalDamage *= (1.f + CritDamage);
	}

	FinalDamage = FMath::Max<float>(0.f, FinalDamage);

	// 데미지 적용
	const FGameplayModifierEvaluatedData EvaluatedData(UF1AttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, FinalDamage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}