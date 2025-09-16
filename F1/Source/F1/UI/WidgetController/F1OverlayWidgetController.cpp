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
}

void UF1OverlayWidgetController::BindCallbacksToDependencies()
{
	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetHealthAttribute()).AddUObject(this, &UF1OverlayWidgetController::HealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UF1OverlayWidgetController::MaxHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetManaAttribute()).AddUObject(this, &UF1OverlayWidgetController::ManaChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMaxManaAttribute()).AddUObject(this, &UF1OverlayWidgetController::MaxManaChanged);
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
