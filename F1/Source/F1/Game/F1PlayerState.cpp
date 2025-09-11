// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/F1PlayerState.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"

AF1PlayerState::AF1PlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UF1AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UF1AttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* AF1PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
