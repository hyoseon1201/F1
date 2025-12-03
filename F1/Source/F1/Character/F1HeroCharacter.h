// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/F1CharacterBase.h"
#include "Data/F1CharacterTypes.h"
#include "F1HeroCharacter.generated.h"

/**
 * 
 */
UCLASS()
class F1_API AF1HeroCharacter : public AF1CharacterBase
{
	GENERATED_BODY()
public:
	AF1HeroCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintPure, Category = "Character")
	FCharacterClassInfo GetCurrentCharacterInfo() const { return CurrentCharacterInfo; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetCharacterClass(FName CharacterRowName);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Data")
	UDataTable* CharacterClassDataTable = nullptr;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentCharacterInfo, Category = "Character")
	FCharacterClassInfo CurrentCharacterInfo;

	UFUNCTION()
	void OnRep_CurrentCharacterInfo();

	void ApplyVisualsFromCurrentInfo();
	void SyncMovementSpeedWithAttributeSet();

	// ===========================================
	// Combat System
	// ===========================================
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	float LifeSpan = 5.f;

	virtual void UpdateCombatSocketsFromCharacterInfo() override;

private:
	virtual void InitAbilityActorInfo() override;

	// ===========================================
	// Combat Interface
	// ===========================================
public:
	virtual int32 GetCurrentLevel() const override;
	virtual float GetCurrentExperience() const override;
	virtual void ApplyLevelBasedGrowth() override;

	virtual FVector GetCombatSocketLocation() override;

	virtual void Die() override;

	// ===========================================
	// Wolrd Widget
	// ===========================================

protected:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UF1WidgetController> HeroWidgetControllerClass;

	// 위젯 컴포넌트 (머리 위 체력바)
	// (BaseCharacter에 있다면 생략 가능)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UWidgetComponent> PlayerBar;

	UPROPERTY()
	FTimerHandle HUDInitTimerHandle;

	virtual void InitUI() override;
};
