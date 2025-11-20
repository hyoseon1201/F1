// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/F1ExCalcMagicalDamage.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "GameplayTag/F1GameplayTags.h"

struct F1MagicalDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicPenetrationFlat);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MagicPenetrationPercent);

	F1MagicalDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, MagicResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, MagicPenetrationFlat, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, MagicPenetrationPercent, Source, false);
	}
};

static const F1MagicalDamageStatics& MagicalDamageStatics()
{
	static F1MagicalDamageStatics MDStatics;

	return MDStatics;
}

UF1ExCalcMagicalDamage::UF1ExCalcMagicalDamage()
{
	RelevantAttributesToCapture.Add(MagicalDamageStatics().MagicResistanceDef);
	RelevantAttributesToCapture.Add(MagicalDamageStatics().MagicPenetrationFlatDef);
	RelevantAttributesToCapture.Add(MagicalDamageStatics().MagicPenetrationPercentDef);
}

void UF1ExCalcMagicalDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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

	// 기본 마법 피해
	float Damage = Spec.GetSetByCallerMagnitude(FF1GameplayTags::Get().DamageType_Magical);

	// 속성 캡처
	float MagicResistance = 0.f;
	float MagicPenFlat = 0.f;
	float MagicPenPercent = 0.f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MagicalDamageStatics().MagicResistanceDef, EvaluationParameters, MagicResistance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MagicalDamageStatics().MagicPenetrationFlatDef, EvaluationParameters, MagicPenFlat);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MagicalDamageStatics().MagicPenetrationPercentDef, EvaluationParameters, MagicPenPercent);

	// 방어력 계산
	float MagicResistanceAfterPercent = MagicResistance * (1.f - FMath::Clamp(MagicPenPercent, 0.f, 1.f));
	float EffectiveMagicResistance = FMath::Max<float>(0.f, MagicResistanceAfterPercent - MagicPenFlat);
	float DamageMultiplier = 100.f / (100.f + EffectiveMagicResistance);
	float FinalDamage = Damage * DamageMultiplier;

	FinalDamage = FMath::Max<float>(0.f, FinalDamage);

	// 데미지 적용
	const FGameplayModifierEvaluatedData EvaluatedData(UF1AttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, FinalDamage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
