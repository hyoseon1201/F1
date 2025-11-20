// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/F1MonsterCharacter.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/F1AbilitySystemLibrary.h"
#include "AI/F1AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "F1.h"

AF1MonsterCharacter::AF1MonsterCharacter()
{
    AbilitySystemComponent = CreateDefaultSubobject<UF1AbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    AttributeSet = CreateDefaultSubobject<UF1AttributeSet>("AttributeSet");
}

void AF1MonsterCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (!HasAuthority()) return;
    F1AIController = Cast<AF1AIController>(NewController);
    F1AIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
    F1AIController->RunBehaviorTree(BehaviorTree);
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

    // 1. 서버 전용: Attributes 및 Abilities 부여 (Library 사용)
    if (HasAuthority())
    {
        UF1AbilitySystemLibrary::InitializeDefaultAttributes(this);
        UF1AbilitySystemLibrary::AddCharacterAbilities(this);
    }
}

void AF1MonsterCharacter::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
}
