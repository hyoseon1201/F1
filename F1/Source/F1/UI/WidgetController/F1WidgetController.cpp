// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/F1WidgetController.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"

void UF1WidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UF1WidgetController::BroadcastInitialValues()
{
}

void UF1WidgetController::BindCallbacksToDependencies()
{
}

UF1AbilitySystemComponent* UF1WidgetController::GetF1ASC()
{
	if (F1AbilitySystemComponent == nullptr)
	{
		F1AbilitySystemComponent = Cast<UF1AbilitySystemComponent>(AbilitySystemComponent);
	}
	return F1AbilitySystemComponent;
}
