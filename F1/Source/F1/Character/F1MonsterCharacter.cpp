// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/F1MonsterCharacter.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"

AF1MonsterCharacter::AF1MonsterCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UF1AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UF1AttributeSet>("AttributeSet");
}
