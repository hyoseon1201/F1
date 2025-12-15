// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/ExecCalc/F1ExCalcBasicDamage.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "GameplayTag/F1GameplayTags.h"
#include "F1AbilityTypes.h"
#include "AbilitySystem/F1AbilitySystemLibrary.h"

// 정적 구조체
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

UF1ExCalcBasicDamage::UF1ExCalcBasicDamage()
{
	RelevantAttributesToCapture.Add(BasicDamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(BasicDamageStatics().ArmorPenetrationFlatDef);
	RelevantAttributesToCapture.Add(BasicDamageStatics().ArmorPenetrationPercentDef);
	RelevantAttributesToCapture.Add(BasicDamageStatics().CriticalStrikeChanceDef);
	RelevantAttributesToCapture.Add(BasicDamageStatics().CriticalStrikeDamageDef);
}

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

	// =========================================================================================
	// [DEBUG] 1. 데미지 수신 확인 (가장 중요!)
	// =========================================================================================
	// 중요: 여기서 DamageType_Physical 태그로 값을 찾고 있습니다.
	// 만약 Ability에서 보낸 태그가 'Magic'이나 다른 것이라면 여기서 0이 나옵니다.
	float Damage = Spec.GetSetByCallerMagnitude(FF1GameplayTags::Get().DamageType_Physical, false, -1.0f);

	UE_LOG(LogTemp, Warning, TEXT("[ExecCalc] ========================================="));

	if (Damage < 0.f)
	{
		// -1.0f가 나왔다면 태그를 못 찾았다는 뜻입니다. (0으로 보정)
		UE_LOG(LogTemp, Error, TEXT("[ExecCalc] 1. FAILED: Could not find 'DamageType.Physical' tag in Spec! Input Damage: %f"), Damage);
		Damage = 0.f;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[ExecCalc] 1. Input Damage (Physical): %f"), Damage);
	}

	// =========================================================================================
	// [DEBUG] 2. 스탯 캡처 확인
	// =========================================================================================
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

	UE_LOG(LogTemp, Log, TEXT("[ExecCalc] 2. Stats Captured -> Armor: %f | PenFlat: %f | PenPct: %f | CritChance: %f | CritDmgMultiplier: %f"),
		Armor, ArmorPenFlat, ArmorPenPercent, CritChance, CritDamage);

	// =========================================================================================
	// [DEBUG] 3. 데미지 공식 계산 확인
	// =========================================================================================

	// 방어력 계산
	float ArmorAfterPercent = Armor * (1.f - FMath::Clamp(ArmorPenPercent, 0.f, 1.f));
	float EffectiveArmor = FMath::Max<float>(0.f, ArmorAfterPercent - ArmorPenFlat);

	// 방어력으로 인한 감소율 (LoL 공식: 100 / (100 + 방어력))
	float DamageMultiplier = 100.f / (100.f + EffectiveArmor);
	float FinalDamage = Damage * DamageMultiplier;

	UE_LOG(LogTemp, Log, TEXT("[ExecCalc] 3. Armor Calc -> EffectiveArmor: %f | Multiplier: %f | Damage After Armor: %f"),
		EffectiveArmor, DamageMultiplier, FinalDamage);

	// 치명타 판정
	bool bIsCritical = FMath::FRand() < FMath::Clamp(CritChance, 0.f, 1.f);

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UF1AbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bIsCritical);

	if (bIsCritical)
	{
		FinalDamage *= (1.f + CritDamage); // 예: CritDamage가 0.5면 1.5배
		UE_LOG(LogTemp, Warning, TEXT("[ExecCalc] 4. CRITICAL HIT! Damage Multiplied."));
	}

	FinalDamage = FMath::Max<float>(0.f, FinalDamage);

	// =========================================================================================
	// [DEBUG] 4. 최종 결과 확인
	// =========================================================================================
	UE_LOG(LogTemp, Warning, TEXT("[ExecCalc] 5. FINAL OUTPUT DAMAGE: %f"), FinalDamage);
	UE_LOG(LogTemp, Warning, TEXT("[ExecCalc] ========================================="));

	if (FinalDamage > 0.f)
	{
		const FGameplayModifierEvaluatedData EvaluatedData(UF1AttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, FinalDamage);
		OutExecutionOutput.AddOutputModifier(EvaluatedData);
	}
}