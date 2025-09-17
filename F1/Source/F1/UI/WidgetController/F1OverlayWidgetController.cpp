// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/F1OverlayWidgetController.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"

void UF1OverlayWidgetController::BroadcastInitialValues()
{
	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);

	// ����Ż
	OnHealthChanged.Broadcast(F1AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(F1AttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(F1AttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(F1AttributeSet->GetMaxMana());

	// ���� �ɷ�ġ
	OnAttackDamageChanged.Broadcast(F1AttributeSet->GetAttackDamage());
	OnAttackSpeedChanged.Broadcast(F1AttributeSet->GetAttackSpeed());
	OnAbilityPowerChanged.Broadcast(F1AttributeSet->GetAbilityPower());
	OnCriticalStrikeChanceChanged.Broadcast(F1AttributeSet->GetCriticalStrikeChance());
	OnCriticalStrikeDamageChanged.Broadcast(F1AttributeSet->GetCriticalStrikeDamage());

	// ��� �ɷ�ġ
	OnArmorChanged.Broadcast(F1AttributeSet->GetArmor());
	OnMagicResistanceChanged.Broadcast(F1AttributeSet->GetMagicResistance());

	// �̵� �� ��ƿ��Ƽ
	OnMovementSpeedChanged.Broadcast(F1AttributeSet->GetMovementSpeed());
	OnAbilityHasteChanged.Broadcast(F1AttributeSet->GetAbilityHaste());

	// �����
	OnArmorPenetrationChanged.Broadcast(F1AttributeSet->GetArmorPenetration());
	OnMagicPenetrationChanged.Broadcast(F1AttributeSet->GetMagicPenetration());

	// ����
	OnLifeStealChanged.Broadcast(F1AttributeSet->GetLifeSteal());
	OnOmnivampChanged.Broadcast(F1AttributeSet->GetOmnivamp());

	// ���׷�
	OnTenacityChanged.Broadcast(F1AttributeSet->GetTenacity());
	OnSlowResistanceChanged.Broadcast(F1AttributeSet->GetSlowResistance());

	// ��Ÿ�
	OnAttackRangeChanged.Broadcast(F1AttributeSet->GetAttackRange());
}

void UF1OverlayWidgetController::BindCallbacksToDependencies()
{
	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);

	// ����Ż ���ε�
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetHealthAttribute()).AddUObject(this, &UF1OverlayWidgetController::HealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UF1OverlayWidgetController::MaxHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetManaAttribute()).AddUObject(this, &UF1OverlayWidgetController::ManaChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMaxManaAttribute()).AddUObject(this, &UF1OverlayWidgetController::MaxManaChanged);

	// ���� �ɷ�ġ ���ε�
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAttackDamageAttribute()).AddUObject(this, &UF1OverlayWidgetController::AttackDamageChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAttackSpeedAttribute()).AddUObject(this, &UF1OverlayWidgetController::AttackSpeedChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAbilityPowerAttribute()).AddUObject(this, &UF1OverlayWidgetController::AbilityPowerChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetCriticalStrikeChanceAttribute()).AddUObject(this, &UF1OverlayWidgetController::CriticalStrikeChanceChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetCriticalStrikeDamageAttribute()).AddUObject(this, &UF1OverlayWidgetController::CriticalStrikeDamageChanged);

	// ��� �ɷ�ġ ���ε�
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetArmorAttribute()).AddUObject(this, &UF1OverlayWidgetController::ArmorChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMagicResistanceAttribute()).AddUObject(this, &UF1OverlayWidgetController::MagicResistanceChanged);

	// �̵� �� ��ƿ��Ƽ ���ε�
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMovementSpeedAttribute()).AddUObject(this, &UF1OverlayWidgetController::MovementSpeedChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAbilityHasteAttribute()).AddUObject(this, &UF1OverlayWidgetController::AbilityHasteChanged);

	// ����� ���ε�
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetArmorPenetrationAttribute()).AddUObject(this, &UF1OverlayWidgetController::ArmorPenetrationChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMagicPenetrationAttribute()).AddUObject(this, &UF1OverlayWidgetController::MagicPenetrationChanged);

	// ���� ���ε�
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetLifeStealAttribute()).AddUObject(this, &UF1OverlayWidgetController::LifeStealChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetOmnivampAttribute()).AddUObject(this, &UF1OverlayWidgetController::OmnivampChanged);

	// ���׷� ���ε�
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetTenacityAttribute()).AddUObject(this, &UF1OverlayWidgetController::TenacityChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetSlowResistanceAttribute()).AddUObject(this, &UF1OverlayWidgetController::SlowResistanceChanged);

	// ��Ÿ� ���ε�
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAttackRangeAttribute()).AddUObject(this, &UF1OverlayWidgetController::AttackRangeChanged);


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

void UF1OverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);
	float SafeHealth = FMath::Clamp(Data.NewValue, 0.0f, F1AttributeSet->GetMaxHealth());
	OnHealthChanged.Broadcast(SafeHealth);
}

void UF1OverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);
	float SafeMana = FMath::Clamp(Data.NewValue, 0.0f, F1AttributeSet->GetMaxMana());
	OnManaChanged.Broadcast(SafeMana);
}

void UF1OverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::AttackDamageChanged(const FOnAttributeChangeData& Data) const
{
	OnAttackDamageChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::AttackSpeedChanged(const FOnAttributeChangeData& Data) const
{
	OnAttackSpeedChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::AbilityPowerChanged(const FOnAttributeChangeData& Data) const
{
	OnAbilityPowerChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::CriticalStrikeChanceChanged(const FOnAttributeChangeData& Data) const
{
	OnCriticalStrikeChanceChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::CriticalStrikeDamageChanged(const FOnAttributeChangeData& Data) const
{
	OnCriticalStrikeDamageChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::ArmorChanged(const FOnAttributeChangeData& Data) const
{
	OnArmorChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::MagicResistanceChanged(const FOnAttributeChangeData& Data) const
{
	OnMagicResistanceChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::MovementSpeedChanged(const FOnAttributeChangeData& Data) const
{
	OnMovementSpeedChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::AbilityHasteChanged(const FOnAttributeChangeData& Data) const
{
	OnAbilityHasteChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::ArmorPenetrationChanged(const FOnAttributeChangeData& Data) const
{
	OnArmorPenetrationChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::MagicPenetrationChanged(const FOnAttributeChangeData& Data) const
{
	OnMagicPenetrationChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::LifeStealChanged(const FOnAttributeChangeData& Data) const
{
	OnLifeStealChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::OmnivampChanged(const FOnAttributeChangeData& Data) const
{
	OnOmnivampChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::TenacityChanged(const FOnAttributeChangeData& Data) const
{
	OnTenacityChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::SlowResistanceChanged(const FOnAttributeChangeData& Data) const
{
	OnSlowResistanceChanged.Broadcast(Data.NewValue);
}

void UF1OverlayWidgetController::AttackRangeChanged(const FOnAttributeChangeData& Data) const
{
	OnAttackRangeChanged.Broadcast(Data.NewValue);
}