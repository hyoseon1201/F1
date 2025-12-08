// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/F1HeroWidgetController.h"
#include "Data/F1AbilityInfo.h"
#include "GameplayTagContainer.h"
#include "F1OverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FF1AbilityInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCooldownChangeSignature, FGameplayTag, CooldownTag, float, TimeRemaining, FGameplayTag, InputTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, UTexture2D*, Image);

// 전방 선언
struct FGameplayEffectSpec;
struct FActiveGameplayEffectHandle;

/**
 * HUD(오버레이) 전용 컨트롤러
 * 경험치, 골드, 초상화, 스킬 아이콘, 쿨타임 정보를 모두 관리합니다.
 */
UCLASS(BlueprintType, Blueprintable)
class F1_API UF1OverlayWidgetController : public UF1HeroWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	// ==============================================================
	// [기존] 추가 스탯 델리게이트들 (생략 없이 유지)
	// ==============================================================
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

	// ==============================================================
	// [이사 옴] 스킬 및 쿨타임 관련 델리게이트
	// ==============================================================
	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FOnCooldownChangeSignature OnCooldownChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnGoldChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnXPChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnMaxXPChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnPlayerStatChangedSignature OnPlayerIconChanged;

protected:
	// [이사 옴] 스킬 아이콘 정보가 담긴 데이터 에셋
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<class UF1AbilityData> AbilityInfo;

	// [이사 옴] 쿨타임 태그가 변했을 때 호출될 콜백 함수
	void OnAbilityTagsChanged(const FGameplayTag CallbackTag, int32 NewCount);

	// [이사 옴] 이펙트 적용 시점 감지 (클라이언트 타이밍 이슈 해결용)
	void OnActiveGameplayEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle);
};