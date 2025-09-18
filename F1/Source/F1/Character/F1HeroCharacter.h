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

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintPure, Category = "Character")
	FCharacterClassInfo GetCurrentCharacterInfo() const { return CurrentCharacterInfo; }

	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetCharacterClass(FName CharacterRowName);

	UFUNCTION(BlueprintPure, Category = "Character Level")
	int32 GetCurrentLevel() const;

	UFUNCTION(BlueprintPure, Category = "Character Level")
	float GetCurrentExperience() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character Data")
	UDataTable* CharacterClassDataTable = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	FCharacterClassInfo CurrentCharacterInfo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<UGameplayEffect> GrowthAttributes;

	void ApplyLevelUpGrowth();

private:
	virtual void InitAbilityActorInfo() override;
};
