// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/F1DamageGameplayAbility.h"
#include "F1Melee.generated.h"

/**
 * 
 */
UCLASS()
class F1_API UF1Melee : public UF1DamageGameplayAbility
{
	GENERATED_BODY()
	
protected:
	// 공격 범위 (반경)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee")
	float AttackRadius = 150.f;

	// 공격 시 재생할 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee")
	TObjectPtr<UAnimMontage> MeleeMontage;

	// [핵심] 블루프린트에서 호출할 공격 판정 함수
	// 몽타주의 노티파이(Notify) 시점에 이 함수가 실행됩니다.
	UFUNCTION(BlueprintCallable, Category = "Melee")
	void TriggerMeleeAttack();
};
