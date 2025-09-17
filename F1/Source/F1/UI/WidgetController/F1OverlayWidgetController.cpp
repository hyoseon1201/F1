// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/WidgetController/F1OverlayWidgetController.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "GameplayTag/F1GameplayTags.h"

void UF1OverlayWidgetController::BroadcastInitialValues()
{
	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);

	InitializeAttributeTagMap();

	for (const auto& [Attribute, Tag] : AttributeTagMap)
	{
		float AttributeValue = AbilitySystemComponent->GetNumericAttribute(Attribute);
		OnAttributeChanged.Broadcast(Tag, AttributeValue);
	}
}

void UF1OverlayWidgetController::BindCallbacksToDependencies()
{
	InitializeAttributeTagMap();

	for (const auto& [Attribute, Tag] : AttributeTagMap)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddLambda(
			[this, Tag](const FOnAttributeChangeData& Data)
			{
				float FinalValue = GetClampedAttributeValue(Data, Tag);
				OnAttributeChanged.Broadcast(Tag, FinalValue);
			}
		);
	}

	// EffectAssetTags 람다
	Cast<UF1AbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, Msg);
			}
		}
	);
}

void UF1OverlayWidgetController::InitializeAttributeTagMap()
{
	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);
	const FF1GameplayTags& GameplayTags = FF1GameplayTags::Get();

	AttributeTagMap = {
		// 생존 능력치
		{F1AttributeSet->GetHealthAttribute(), GameplayTags.Attributes_Vital_Health},
		{F1AttributeSet->GetMaxHealthAttribute(), GameplayTags.Attributes_Vital_MaxHealth},
		{F1AttributeSet->GetHealthRegenerationAttribute(), GameplayTags.Attributes_Vital_HealthRegeneration},
		{F1AttributeSet->GetManaAttribute(), GameplayTags.Attributes_Vital_Mana},
		{F1AttributeSet->GetMaxManaAttribute(), GameplayTags.Attributes_Vital_MaxMana},
		{F1AttributeSet->GetManaRegenerationAttribute(), GameplayTags.Attributes_Vital_ManaRegeneration},

		// 공격 능력치
		{F1AttributeSet->GetAttackDamageAttribute(), GameplayTags.Attributes_Offensive_AttackDamage},
		{F1AttributeSet->GetAttackSpeedAttribute(), GameplayTags.Attributes_Offensive_AttackSpeed},
		{F1AttributeSet->GetAbilityPowerAttribute(), GameplayTags.Attributes_Offensive_AbilityPower},
		{F1AttributeSet->GetCriticalStrikeChanceAttribute(), GameplayTags.Attributes_Offensive_CriticalStrikeChance},
		{F1AttributeSet->GetCriticalStrikeDamageAttribute(), GameplayTags.Attributes_Offensive_CriticalStrikeDamage},

		// 방어 능력치
		{F1AttributeSet->GetArmorAttribute(), GameplayTags.Attributes_Defensive_Armor},
		{F1AttributeSet->GetMagicResistanceAttribute(), GameplayTags.Attributes_Defensive_MagicResistance},

		// 이동 및 유틸리티
		{F1AttributeSet->GetMovementSpeedAttribute(), GameplayTags.Attributes_Movement_Speed},
		{F1AttributeSet->GetAbilityHasteAttribute(), GameplayTags.Attributes_Utility_AbilityHaste},

		// 관통력
		{F1AttributeSet->GetArmorPenetrationAttribute(), GameplayTags.Attributes_Penetration_Armor},
		{F1AttributeSet->GetMagicPenetrationAttribute(), GameplayTags.Attributes_Penetration_Magic},

		// 흡혈
		{F1AttributeSet->GetLifeStealAttribute(), GameplayTags.Attributes_Sustain_LifeSteal},
		{F1AttributeSet->GetOmnivampAttribute(), GameplayTags.Attributes_Sustain_Omnivamp},

		// 저항력
		{F1AttributeSet->GetTenacityAttribute(), GameplayTags.Attributes_Resistance_Tenacity},
		{F1AttributeSet->GetSlowResistanceAttribute(), GameplayTags.Attributes_Resistance_SlowResistance},

		// 사거리
		{F1AttributeSet->GetAttackRangeAttribute(), GameplayTags.Attributes_Range_Attack}
	};
}

float UF1OverlayWidgetController::GetClampedAttributeValue(const FOnAttributeChangeData& Data, const FGameplayTag& AttributeTag) const
{
	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);
	const FF1GameplayTags& GameplayTags = FF1GameplayTags::Get();

	if (AttributeTag.MatchesTagExact(GameplayTags.Attributes_Vital_Health))
	{
		return FMath::Clamp(Data.NewValue, 0.0f, F1AttributeSet->GetMaxHealth());
	}
	else if (AttributeTag.MatchesTagExact(GameplayTags.Attributes_Vital_Mana))
	{
		return FMath::Clamp(Data.NewValue, 0.0f, F1AttributeSet->GetMaxMana());
	}
	else if (AttributeTag.MatchesTagExact(GameplayTags.Attributes_Offensive_CriticalStrikeChance) ||
		AttributeTag.MatchesTagExact(GameplayTags.Attributes_Sustain_LifeSteal) ||
		AttributeTag.MatchesTagExact(GameplayTags.Attributes_Sustain_Omnivamp))
	{
		return FMath::Clamp(Data.NewValue, 0.0f, 100.0f);
	}
	else
	{
		return Data.NewValue;
	}
}
