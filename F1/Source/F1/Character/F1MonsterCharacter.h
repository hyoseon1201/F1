// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/F1CharacterBase.h"
#include "GameplayTagContainer.h"
#include "F1MonsterCharacter.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AF1AIController;

/**
 * 
 */
UCLASS()
class F1_API AF1MonsterCharacter : public AF1CharacterBase
{
	GENERATED_BODY()
public:
	AF1MonsterCharacter();
	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	// ===========================================
	// Combat System
	// ===========================================
protected:
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	//float LifeSpan = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DefaultAttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	FGameplayTag AttackAbilityTag;

	// ===========================================
	// Combat Interface
	// ===========================================
public:
	virtual int32 GetRewardXP_Implementation() override;
	virtual int32 GetRewardGold_Implementation() override;

	virtual void Die() override;
	virtual void SetCombatTarget(AActor* InTarget) override;
	virtual void Attack() override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	virtual void MulticastHandleDeath_Implementation() override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Reward")
	int32 RewardXP = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Reward")
	int32 RewardGold = 100;

	// ===========================================
	// AI
	// ===========================================
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AF1AIController> F1AIController;

	// ===========================================
	// Wolrd Widget
	// ===========================================

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	bool bHealthBarInitialized = false;

	virtual void InitUI() override;

};
