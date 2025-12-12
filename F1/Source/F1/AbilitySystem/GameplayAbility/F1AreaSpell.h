// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/F1DamageGameplayAbility.h"
#include "F1AreaSpell.generated.h"

class AF1MeteorCircle;

/**
 * 
 */
UCLASS()
class F1_API UF1AreaSpell : public UF1DamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	// 블루프린트에서 호출할 메인 함수
	UFUNCTION(BlueprintCallable, Category = "Area")
	void SpawnAreaActor(const FVector& TargetLocation);

protected:
	// 소환할 장판 액터 클래스 (BP에서 설정: BP_MeteorCircle)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Area")
	TSubclassOf<AF1MeteorCircle> AreaActorClass;
};
