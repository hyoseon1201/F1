// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/WidgetController/F1OverlayWidgetController.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"

void UF1OverlayWidgetController::BroadcastInitialValues()
{
	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(F1AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(F1AttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(F1AttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(F1AttributeSet->GetMaxMana());
	OnHealthRegenerationChanged.Broadcast(F1AttributeSet->GetHealthRegeneration());
	OnManaRegenerationChanged.Broadcast(F1AttributeSet->GetManaRegeneration());

	OnAttackDamageChanged.Broadcast(F1AttributeSet->GetAttackDamage());
	OnAttackSpeedChanged.Broadcast(F1AttributeSet->GetAttackSpeed());
	OnAbilityPowerChanged.Broadcast(F1AttributeSet->GetAbilityPower());
	OnCriticalStrikeChanceChanged.Broadcast(F1AttributeSet->GetCriticalStrikeChance());
	OnCriticalStrikeDamageChanged.Broadcast(F1AttributeSet->GetCriticalStrikeDamage());

	OnArmorChanged.Broadcast(F1AttributeSet->GetArmor());
	OnMagicResistanceChanged.Broadcast(F1AttributeSet->GetMagicResistance());

	OnMovementSpeedChanged.Broadcast(F1AttributeSet->GetMovementSpeed());
	OnAbilityHasteChanged.Broadcast(F1AttributeSet->GetAbilityHaste());

	OnArmorPenetrationFlatChanged.Broadcast(F1AttributeSet->GetArmorPenetrationFlat());
	OnArmorPenetrationPercentChanged.Broadcast(F1AttributeSet->GetArmorPenetrationPercent());
	OnMagicPenetrationFlatChanged.Broadcast(F1AttributeSet->GetMagicPenetrationFlat());
	OnMagicPenetrationPercentChanged.Broadcast(F1AttributeSet->GetMagicPenetrationPercent());

	OnLifeStealChanged.Broadcast(F1AttributeSet->GetLifeSteal());
	OnOmnivampChanged.Broadcast(F1AttributeSet->GetOmnivamp());

	OnTenacityChanged.Broadcast(F1AttributeSet->GetTenacity());
	OnSlowResistanceChanged.Broadcast(F1AttributeSet->GetSlowResistance());

	OnAttackRangeChanged.Broadcast(F1AttributeSet->GetAttackRange());
}

void UF1OverlayWidgetController::BindCallbacksToDependencies()
{
	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);

	// Health - 클램핑 처리
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetHealthAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				const UF1AttributeSet* AS = CastChecked<UF1AttributeSet>(AttributeSet);
				float SafeHealth = FMath::Clamp(Data.NewValue, 0.0f, AS->GetMaxHealth());
				OnHealthChanged.Broadcast(SafeHealth);
			});

	// Max Health
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMaxHealthAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});

	// Mana - 클램핑 처리
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetManaAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				const UF1AttributeSet* AS = CastChecked<UF1AttributeSet>(AttributeSet);
				float SafeMana = FMath::Clamp(Data.NewValue, 0.0f, AS->GetMaxMana());
				OnManaChanged.Broadcast(SafeMana);
			});

	// Max Mana
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMaxManaAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			});

	// Health Regeneration
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetHealthRegenerationAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnHealthRegenerationChanged.Broadcast(Data.NewValue);
			});

	// Mana Regeneration
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetManaRegenerationAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnManaRegenerationChanged.Broadcast(Data.NewValue);
			});

	// Attack Damage
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAttackDamageAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnAttackDamageChanged.Broadcast(Data.NewValue);
			});

	// Attack Speed
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAttackSpeedAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnAttackSpeedChanged.Broadcast(Data.NewValue);
			});

	// Ability Power
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAbilityPowerAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnAbilityPowerChanged.Broadcast(Data.NewValue);
			});

	// Critical Strike Chance
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetCriticalStrikeChanceAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnCriticalStrikeChanceChanged.Broadcast(Data.NewValue);
			});

	// Critical Strike Damage
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetCriticalStrikeDamageAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnCriticalStrikeDamageChanged.Broadcast(Data.NewValue);
			});

	// Armor
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetArmorAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnArmorChanged.Broadcast(Data.NewValue);
			});

	// Magic Resistance
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMagicResistanceAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMagicResistanceChanged.Broadcast(Data.NewValue);
			});

	// Movement Speed
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMovementSpeedAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMovementSpeedChanged.Broadcast(Data.NewValue);
			});

	// Ability Haste
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAbilityHasteAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnAbilityHasteChanged.Broadcast(Data.NewValue);
			});

	// Armor Penetration Flat
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetArmorPenetrationFlatAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnArmorPenetrationFlatChanged.Broadcast(Data.NewValue);
			});

	// Armor Penetration Percent
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetArmorPenetrationPercentAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnArmorPenetrationPercentChanged.Broadcast(Data.NewValue);
			});

	// Magic Penetration Flat
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMagicPenetrationFlatAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMagicPenetrationFlatChanged.Broadcast(Data.NewValue);
			});

	// Magic Penetration Percent
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMagicPenetrationPercentAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMagicPenetrationPercentChanged.Broadcast(Data.NewValue);
			});

	// Life Steal
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetLifeStealAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnLifeStealChanged.Broadcast(Data.NewValue);
			});

	// Omnivamp
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetOmnivampAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnOmnivampChanged.Broadcast(Data.NewValue);
			});

	// Tenacity
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetTenacityAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnTenacityChanged.Broadcast(Data.NewValue);
			});

	// Slow Resistance
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetSlowResistanceAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnSlowResistanceChanged.Broadcast(Data.NewValue);
			});

	// Attack Range
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAttackRangeAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnAttackRangeChanged.Broadcast(Data.NewValue);
			});

	// EffectAssetTags 람다
	Cast<UF1AbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Msg);
			}
		}
	);
}
