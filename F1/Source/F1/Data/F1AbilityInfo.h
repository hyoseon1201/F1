// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h" // 태그 사용을 위해 필수
#include "F1AbilityInfo.generated.h"

/**
 * 개별 스킬의 UI 정보를 담는 구조체
 */
USTRUCT(BlueprintType)
struct FF1AbilityInfo
{
	GENERATED_BODY()

	// 1. 어떤 스킬인가? (예: Ability.Skill.FireBolt)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();

	// 2. 무슨 키로 발동하는가? (예: InputTag.Q)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();

	// 3. 쿨타임 태그는 무엇인가? (예: Cooldown.Skill.FireBolt)
	// (나중에 쿨타임 UI 돌릴 때 필요함)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CooldownTag = FGameplayTag();

	// 4. 아이콘 이미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;
};

/**
 * 캐릭터별 스킬 정보 목록을 관리하는 데이터 에셋
 */
UCLASS()
class F1_API UF1AbilityData : public UDataAsset
{
	GENERATED_BODY()

public:
	// 에디터에서 채워넣을 배열
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInformation")
	TArray<FF1AbilityInfo> AbilityInformation;

	// 태그를 주면 해당 정보를 찾아서 반환하는 헬퍼 함수
	UFUNCTION(BlueprintCallable, Category = "AbilityInformation")
	FF1AbilityInfo FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound = false) const;
};