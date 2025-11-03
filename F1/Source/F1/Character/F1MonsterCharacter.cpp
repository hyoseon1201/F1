// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/F1MonsterCharacter.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "F1.h"

AF1MonsterCharacter::AF1MonsterCharacter()
{
    AbilitySystemComponent = CreateDefaultSubobject<UF1AbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    AttributeSet = CreateDefaultSubobject<UF1AttributeSet>("AttributeSet");
}

void AF1MonsterCharacter::BeginPlay()
{
    Super::BeginPlay();

    InitAbilityActorInfo();
    InitializeHealthBarWidget();
}

void AF1MonsterCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UF1AbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if (HasAuthority())
	{
		InitializeDefaultAttributes();
		AddCharacterAbilities();
	}
}

void AF1MonsterCharacter::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
}
