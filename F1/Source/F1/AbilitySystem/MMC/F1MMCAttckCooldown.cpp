// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MMC/F1MMCAttckCooldown.h"
#include "AbilitySystem/F1AttributeSet.h"

UF1MMCAttckCooldown::UF1MMCAttckCooldown()
{
	AttackSpeedDef.AttributeToCapture = UF1AttributeSet::GetAttackSpeedAttribute();
	AttackSpeedDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	AttackSpeedDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(AttackSpeedDef);
}

float UF1MMCAttckCooldown::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float AttackSpeed = 0.f;
	GetCapturedAttributeMagnitude(AttackSpeedDef, Spec, EvaluationParameters, AttackSpeed);

	// [디버깅 로그] 들어온 공속 값 확인
	UE_LOG(LogTemp, Warning, TEXT("=========================================="));
	UE_LOG(LogTemp, Warning, TEXT("[MMC] Raw AttackSpeed Captured: %f"), AttackSpeed);

	// 0 방지
	AttackSpeed = FMath::Max(AttackSpeed, 0.01f);

	float CooldownDuration = 1.f / AttackSpeed;

	// [디버깅 로그] 최종 계산된 쿨타임 확인
	UE_LOG(LogTemp, Warning, TEXT("[MMC] Calculated Cooldown: 1.0 / %f = %f seconds"), AttackSpeed, CooldownDuration);
	UE_LOG(LogTemp, Warning, TEXT("=========================================="));

	return CooldownDuration;
}
