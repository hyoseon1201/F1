// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/F1WidgetController.h"
#include "F1OverlayWidgetController.generated.h"

// 바이탈
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChangedSignature, float, NewMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChangedSignature, float, NewMaxMana);

// 공격 능력치
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackDamageChangedSignature, float, NewAttackDamage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttackSpeedChangedSignature, float, NewAttackSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityPowerChangedSignature, float, NewAbilityPower);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCriticalStrikeChanceChangedSignature, float, NewCriticalStrikeChance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCriticalStrikeDamageChangedSignature, float, NewCriticalStrikeDamage);

// 방어 능력치
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArmorChangedSignature, float, NewArmor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMagicResistanceChangedSignature, float, NewMagicResistance);

// 이동 및 유틸리티
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementSpeedChangedSignature, float, NewMovementSpeed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityHasteChangedSignature, float, NewAbilityHaste);

// 관통력
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnArmorPenetrationChangedSignature, float, NewArmorPenetration);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMagicPenetrationChangedSignature, float, NewMagicPenetration);

// 흡혈
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLifeStealChangedSignature, float, NewLifeSteal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOmnivampChangedSignature, float, NewOmnivamp);

// 저항력
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTenacityChangedSignature, float, NewTenacity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlowResistanceChangedSignature, float, NewSlowResistance);

// 사거리
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

	// 바이탈
	UPROPERTY(BlueprintAssignable, Category = "GAS|Vital")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Vital")
	FOnMaxHealthChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Vital")
	FOnManaChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Vital")
	FOnMaxManaChangedSignature OnMaxManaChanged;

	// 공격 능력치
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

	// 방어 능력치
	UPROPERTY(BlueprintAssignable, Category = "GAS|Defense")
	FOnArmorChangedSignature OnArmorChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Defense")
	FOnMagicResistanceChangedSignature OnMagicResistanceChanged;

	// 이동 및 유틸리티
	UPROPERTY(BlueprintAssignable, Category = "GAS|Utility")
	FOnMovementSpeedChangedSignature OnMovementSpeedChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Utility")
	FOnAbilityHasteChangedSignature OnAbilityHasteChanged;

	// 관통력
	UPROPERTY(BlueprintAssignable, Category = "GAS|Penetration")
	FOnArmorPenetrationChangedSignature OnArmorPenetrationChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Penetration")
	FOnMagicPenetrationChangedSignature OnMagicPenetrationChanged;

	// 흡혈
	UPROPERTY(BlueprintAssignable, Category = "GAS|Sustain")
	FOnLifeStealChangedSignature OnLifeStealChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Sustain")
	FOnOmnivampChangedSignature OnOmnivampChanged;

	// 저항력
	UPROPERTY(BlueprintAssignable, Category = "GAS|Resistance")
	FOnTenacityChangedSignature OnTenacityChanged;
	UPROPERTY(BlueprintAssignable, Category = "GAS|Resistance")
	FOnSlowResistanceChangedSignature OnSlowResistanceChanged;

	// 사거리
	UPROPERTY(BlueprintAssignable, Category = "GAS|Range")
	FOnAttackRangeChangedSignature OnAttackRangeChanged;

protected:
	// 바이탈
	void HealthChanged(const FOnAttributeChangeData& Data) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Data) const;
	void ManaChanged(const FOnAttributeChangeData& Data) const;
	void MaxManaChanged(const FOnAttributeChangeData& Data) const;

	// 공격 능력치
	void AttackDamageChanged(const FOnAttributeChangeData& Data) const;
	void AttackSpeedChanged(const FOnAttributeChangeData& Data) const;
	void AbilityPowerChanged(const FOnAttributeChangeData& Data) const;
	void CriticalStrikeChanceChanged(const FOnAttributeChangeData& Data) const;
	void CriticalStrikeDamageChanged(const FOnAttributeChangeData& Data) const;

	// 방어 능력치
	void ArmorChanged(const FOnAttributeChangeData& Data) const;
	void MagicResistanceChanged(const FOnAttributeChangeData& Data) const;

	// 이동 및 유틸리티
	void MovementSpeedChanged(const FOnAttributeChangeData& Data) const;
	void AbilityHasteChanged(const FOnAttributeChangeData& Data) const;

	// 관통력
	void ArmorPenetrationChanged(const FOnAttributeChangeData& Data) const;
	void MagicPenetrationChanged(const FOnAttributeChangeData& Data) const;

	// 흡혈
	void LifeStealChanged(const FOnAttributeChangeData& Data) const;
	void OmnivampChanged(const FOnAttributeChangeData& Data) const;

	// 저항력
	void TenacityChanged(const FOnAttributeChangeData& Data) const;
	void SlowResistanceChanged(const FOnAttributeChangeData& Data) const;

	// 사거리
	void AttackRangeChanged(const FOnAttributeChangeData& Data) const;
};
