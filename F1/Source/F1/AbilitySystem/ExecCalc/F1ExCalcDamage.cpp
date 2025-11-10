// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/F1ExCalcDamage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"

struct F1DamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);

	F1DamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UF1AttributeSet, Armor, Target, false);
	}
};

static const F1DamageStatics& DamageStatics()
{
	static F1DamageStatics DStatics;

	return DStatics;
}

UF1ExCalcDamage::UF1ExCalcDamage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}

void UF1ExCalcDamage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
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

	float Armor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, Armor);
	Armor = FMath::Max<float>(0.f, Armor);
	++Armor;

	const FGameplayModifierEvaluatedData EvaluatedData(DamageStatics().ArmorProperty, EGameplayModOp::Additive, Armor);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
