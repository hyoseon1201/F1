// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/F1MonsterCharacter.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/F1AbilitySystemLibrary.h"
#include "AI/F1AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "F1.h"
#include <GameplayTag/F1GameplayTags.h>

AF1MonsterCharacter::AF1MonsterCharacter()
{
    AbilitySystemComponent = CreateDefaultSubobject<UF1AbilitySystemComponent>("AbilitySystemComponent");
    AbilitySystemComponent->SetIsReplicated(true);
    AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

    AttributeSet = CreateDefaultSubobject<UF1AttributeSet>("AttributeSet");

    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AF1MonsterCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    if (!HasAuthority()) return;
    F1AIController = Cast<AF1AIController>(NewController);
    F1AIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
    F1AIController->GetBlackboardComponent()->SetValueAsVector(FName("HomeLocation"), GetActorLocation());
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

void AF1MonsterCharacter::SetCombatTarget(AActor* InTarget)
{
    if (F1AIController && F1AIController->GetBlackboardComponent())
    {
        F1AIController->GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), InTarget);
    }
}

void AF1MonsterCharacter::Attack()
{
    const FF1GameplayTags& GameplayTags = FF1GameplayTags::Get();

    FGameplayTag AttackTag = GameplayTags.Ability_BasicAttack;

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
    }
}
