// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/F1OverlayWidgetController.h"
#include "AbilitySystem/F1AttributeSet.h"

void UF1OverlayWidgetController::BroadcastInitialValues()
{
	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(F1AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(F1AttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(F1AttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(F1AttributeSet->GetMaxMana());
}
