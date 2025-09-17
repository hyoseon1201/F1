// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/F1WidgetController.h"
#include "F1OverlayWidgetController.generated.h"

// ����Ż
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChangedSignature, float, NewMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChangedSignature, float, NewMaxMana);

// ���� �ɷ�ġ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackDamageChangedSignature, float, NewAttackDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackSpeedChangedSignature, float, NewAttackSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityPowerChangedSignature, float, NewAbilityPower);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCriticalStrikeChanceChangedSignature, float, NewCriticalStrikeChance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCriticalStrikeDamageChangedSignature, float, NewCriticalStrikeDamage);

// ��� �ɷ�ġ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArmorChangedSignature, float, NewArmor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMagicResistanceChangedSignature, float, NewMagicResistance);

// �̵� �� ��ƿ��Ƽ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementSpeedChangedSignature, float, NewMovementSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityHasteChangedSignature, float, NewAbilityHaste);

// �����
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArmorPenetrationChangedSignature, float, NewArmorPenetration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMagicPenetrationChangedSignature, float, NewMagicPenetration);

// ����
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLifeStealChangedSignature, float, NewLifeSteal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOmnivampChangedSignature, float, NewOmnivamp);

// ���׷�
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTenacityChangedSignature, float, NewTenacity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlowResistanceChangedSignature, float, NewSlowResistance);

// ��Ÿ�
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackRangeChangedSignature, float, NewAttackRange);


struct FOnAttributeChangeData;


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class F1_API UF1OverlayWidgetController : public UF1WidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	// ����Ż
	UPROPERTY(BlueprintAssignable, Category = "GAS|Vital")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Vital")
	FOnMaxHealthChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Vital")
	FOnManaChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Vital")
	FOnMaxManaChangedSignature OnMaxManaChanged;

	// ���� �ɷ�ġ
	UPROPERTY(BlueprintAssignable, Category = "GAS|Combat")
	FOnAttackDamageChangedSignature OnAttackDamageChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Combat")
	FOnAttackSpeedChangedSignature OnAttackSpeedChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Combat")
	FOnAbilityPowerChangedSignature OnAbilityPowerChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Combat")
	FOnCriticalStrikeChanceChangedSignature OnCriticalStrikeChanceChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Combat")
	FOnCriticalStrikeDamageChangedSignature OnCriticalStrikeDamageChanged;

	// ��� �ɷ�ġ
	UPROPERTY(BlueprintAssignable, Category = "GAS|Defense")
	FOnArmorChangedSignature OnArmorChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Defense")
	FOnMagicResistanceChangedSignature OnMagicResistanceChanged;

	// �̵� �� ��ƿ��Ƽ
	UPROPERTY(BlueprintAssignable, Category = "GAS|Utility")
	FOnMovementSpeedChangedSignature OnMovementSpeedChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Utility")
	FOnAbilityHasteChangedSignature OnAbilityHasteChanged;

	// �����
	UPROPERTY(BlueprintAssignable, Category = "GAS|Penetration")
	FOnArmorPenetrationChangedSignature OnArmorPenetrationChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Penetration")
	FOnMagicPenetrationChangedSignature OnMagicPenetrationChanged;

	// ����
	UPROPERTY(BlueprintAssignable, Category = "GAS|Sustain")
	FOnLifeStealChangedSignature OnLifeStealChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Sustain")
	FOnOmnivampChangedSignature OnOmnivampChanged;

	// ���׷�
	UPROPERTY(BlueprintAssignable, Category = "GAS|Resistance")
	FOnTenacityChangedSignature OnTenacityChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Resistance")
	FOnSlowResistanceChangedSignature OnSlowResistanceChanged;

	// ��Ÿ�
	UPROPERTY(BlueprintAssignable, Category = "GAS|Range")
	FOnAttackRangeChangedSignature OnAttackRangeChanged;

protected:
	// ����Ż
	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	void ManaChanged(const FOnAttributeChangeData& Data) const;
	void MaxManaChanged(const FOnAttributeChangeData& Data) const;

	// ���� �ɷ�ġ
	void AttackDamageChanged(const FOnAttributeChangeData& Data) const;
	void AttackSpeedChanged(const FOnAttributeChangeData& Data) const;
	void AbilityPowerChanged(const FOnAttributeChangeData& Data) const;
	void CriticalStrikeChanceChanged(const FOnAttributeChangeData& Data) const;
	void CriticalStrikeDamageChanged(const FOnAttributeChangeData& Data) const;

	// ��� �ɷ�ġ
	void ArmorChanged(const FOnAttributeChangeData& Data) const;
	void MagicResistanceChanged(const FOnAttributeChangeData& Data) const;

	// �̵� �� ��ƿ��Ƽ
	void MovementSpeedChanged(const FOnAttributeChangeData& Data) const;
	void AbilityHasteChanged(const FOnAttributeChangeData& Data) const;

	// �����
	void ArmorPenetrationChanged(const FOnAttributeChangeData& Data) const;
	void MagicPenetrationChanged(const FOnAttributeChangeData& Data) const;

	// ����
	void LifeStealChanged(const FOnAttributeChangeData& Data) const;
	void OmnivampChanged(const FOnAttributeChangeData& Data) const;

	// ���׷�
	void TenacityChanged(const FOnAttributeChangeData& Data) const;
	void SlowResistanceChanged(const FOnAttributeChangeData& Data) const;

	// ��Ÿ�
	void AttackRangeChanged(const FOnAttributeChangeData& Data) const;
};
