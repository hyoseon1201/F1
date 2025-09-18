// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/F1WidgetController.h"
#include "F1OverlayWidgetController.generated.h"

struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

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

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthRegenerationChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnManaRegenerationChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Combat")
	FOnAttributeChangedSignature OnAttackDamageChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Combat")
	FOnAttributeChangedSignature OnAttackSpeedChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Combat")
	FOnAttributeChangedSignature OnAbilityPowerChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Combat")
	FOnAttributeChangedSignature OnCriticalStrikeChanceChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Combat")
	FOnAttributeChangedSignature OnCriticalStrikeDamageChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Defense")
	FOnAttributeChangedSignature OnArmorChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Defense")
	FOnAttributeChangedSignature OnMagicResistanceChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Utility")
	FOnAttributeChangedSignature OnMovementSpeedChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Utility")
	FOnAttributeChangedSignature OnAbilityHasteChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Penetration")
	FOnAttributeChangedSignature OnArmorPenetrationFlatChanged;


	UPROPERTY(BlueprintAssignable, Category = "GAS|Penetration")
	FOnAttributeChangedSignature OnArmorPenetrationPercentChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Penetration")
	FOnAttributeChangedSignature OnMagicPenetrationFlatChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Penetration")
	FOnAttributeChangedSignature OnMagicPenetrationPercentChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Sustain")
	FOnAttributeChangedSignature OnLifeStealChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Sustain")
	FOnAttributeChangedSignature OnOmnivampChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Resistance")
	FOnAttributeChangedSignature OnTenacityChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Resistance")
	FOnAttributeChangedSignature OnSlowResistanceChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Range")
	FOnAttributeChangedSignature OnAttackRangeChanged;
};
