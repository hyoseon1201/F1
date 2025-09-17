// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "UI/WidgetController/F1WidgetController.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "F1OverlayWidgetController.generated.h"

struct FOnAttributeChangeData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChangedSignature, const FGameplayTag&, AttributeTag, float, NewValue);

UCLASS(BlueprintType, Blueprintable)
class F1_API UF1OverlayWidgetController : public UF1WidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnAttributeChanged;

protected:
	TMap<FGameplayAttribute, FGameplayTag> AttributeTagMap;

	void InitializeAttributeTagMap();

private:
	float GetClampedAttributeValue(const FOnAttributeChangeData& Data, const FGameplayTag& AttributeTag) const;
};
