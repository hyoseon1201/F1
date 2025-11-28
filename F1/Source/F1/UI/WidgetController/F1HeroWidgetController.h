// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/F1WidgetController.h"
#include "Data/F1AbilityInfo.h"
#include "GameplayTagContainer.h"
#include "F1HeroWidgetController.generated.h"

// ==============================================================
// 델리게이트 선언
// ==============================================================
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangeSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChangeSignature, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNameChangeSignature, const FString&, NewName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FF1AbilityInfo&, Info);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCooldownChangeSignature, FGameplayTag, AbilityTag, float, TimeRemaining);

/**
 * 영웅 캐릭터(플레이어) 전용 UI 컨트롤러
 * 체력, 마나, 레벨, 스킬 아이콘, 스킬 쿨타임 정보를 관리합니다.
 */
UCLASS(BlueprintType, Blueprintable)
class F1_API UF1HeroWidgetController : public UF1WidgetController
{
	GENERATED_BODY()

public:
	// 초기값 방송 (게임 시작 시 1회 호출)
	virtual void BroadcastInitialValues() override;

	// 데이터 변동 감지 (이벤트 구독)
	virtual void BindCallbacksToDependencies() override;

	// ==============================================================
	// [Blueprint Assignable] 위젯에서 구독할 이벤트들
	// ==============================================================

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Status")
	FOnLevelChangeSignature OnLevelChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Status")
	FOnNameChangeSignature OnNameChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Messages")
	FOnCooldownChangeSignature OnCooldownChanged;

protected:
	// 스킬 아이콘 정보가 담긴 데이터 에셋
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<class UF1AbilityData> AbilityInfo;

	// [추가] 쿨타임 태그가 변했을 때 호출될 콜백 함수
	void OnAbilityTagsChanged(const FGameplayTag CallbackTag, int32 NewCount);
};