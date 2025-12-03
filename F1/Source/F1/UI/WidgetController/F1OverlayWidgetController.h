#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/F1HeroWidgetController.h"
#include "F1OverlayWidgetController.generated.h"

// FOnAttributeChangedSignature 선언은 부모나 F1WidgetController로 옮기는 게 좋습니다. 
// 여기서는 중복 정의를 피하기 위해 그대로 두거나 재사용합니다.
// (이미 정의되어 있다면 지우셔도 됩니다.)

UCLASS(BlueprintType, Blueprintable)
class F1_API UF1OverlayWidgetController : public UF1HeroWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	// [중복 삭제] 체력, 마나 등은 부모가 가지고 있으므로 여기선 지웁니다.
	// 오직 Overlay만의 추가 스탯들만 남깁니다.

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