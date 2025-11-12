// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/F1ExCalcPhysicalDamage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "GameplayTag/F1GameplayTags.h"

struct F1PhysicalDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetrationFlat);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetrationPercent);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalStrikeChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalStrikeDamage);

	F1PhysicalDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, ArmorPenetrationFlat, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, ArmorPenetrationPercent, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, CriticalStrikeChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, CriticalStrikeDamage, Source, false);
	}
};

static const F1PhysicalDamageStatics& PhysicalDamageStatics()
{
	static F1PhysicalDamageStatics PDStatics;

	return PDStatics;
}

UF1ExCalcPhysicalDamage::UF1ExCalcPhysicalDamage()
{
	RelevantAttributesToCapture.Add(PhysicalDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(PhysicalDamageStatics().ArmorPenetrationFlatDef);
	RelevantAttributesToCapture.Add(PhysicalDamageStatics().ArmorPenetrationPercentDef);
	RelevantAttributesToCapture.Add(PhysicalDamageStatics().CriticalStrikeChanceDef);
	RelevantAttributesToCapture.Add(PhysicalDamageStatics().CriticalStrikeDamageDef);
}

void UF1ExCalcPhysicalDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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
	float Damage = Spec.GetSetByCallerMagnitude(FF1GameplayTags::Get().DamageType_Physical);

	// 속성 캡처
	float Armor = 0.f;
	float ArmorPenFlat = 0.f;
	float ArmorPenPercent = 0.f;
	float CritChance = 0.f;
	float CritDamage = 0.f;

	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(PhysicalDamageStatics().ArmorDef, EvaluationParameters, Armor);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(PhysicalDamageStatics().ArmorPenetrationFlatDef, EvaluationParameters, ArmorPenFlat);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(PhysicalDamageStatics().ArmorPenetrationPercentDef, EvaluationParameters, ArmorPenPercent);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(PhysicalDamageStatics().CriticalStrikeChanceDef, EvaluationParameters, CritChance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(PhysicalDamageStatics().CriticalStrikeDamageDef, EvaluationParameters, CritDamage);

	// 방어력 계산
	float ArmorAfterPercent = Armor * (1.f - FMath::Clamp(ArmorPenPercent, 0.f, 1.f));
	float EffectiveArmor = FMath::Max<float>(0.f, ArmorAfterPercent - ArmorPenFlat);
	float DamageMultiplier = 100.f / (100.f + EffectiveArmor);
	float FinalDamage = Damage * DamageMultiplier;

	// 치명타 판정
	bool bIsCritical = FMath::FRand() < FMath::Clamp(CritChance, 0.f, 1.f);
	if (bIsCritical)
	{
		FinalDamage *= (1.f + CritDamage);
	}

	FinalDamage = FMath::Max<float>(0.f, FinalDamage);

	// 데미지 적용
	const FGameplayModifierEvaluatedData EvaluatedData(UF1AttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, FinalDamage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

