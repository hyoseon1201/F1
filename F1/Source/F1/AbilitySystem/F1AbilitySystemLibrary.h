// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "F1AbilitySystemLibrary.generated.h"

class UF1OverlayWidgetController;

class UAbilitySystemComponent;
class AActor;
class UGameplayAbility;
class UGameplayEffect;

/**
 * */
UCLASS()
class F1_API UF1AbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ===========================================
	// Widget Controller Functions
	// ===========================================
	UFUNCTION(BlueprintPure, Category = "F1AbilitySystemLibrary|WidgetController")
	static UF1OverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	// ===========================================
	// GAS Initialization Functions
	// ===========================================
	/**
	 * 캐릭터의 Default Attributes를 ASC에 적용합니다. (서버에서만 호출)
	 * @param TargetActor 속성을 초기화할 캐릭터 (AF1CharacterBase 또는 자식 클래스)
	 */
	UFUNCTION(BlueprintCallable, Category = "F1AbilitySystemLibrary|GAS")
	static void InitializeDefaultAttributes(AActor* TargetActor);

	/**
	 * 캐릭터의 Startup Abilities를 ASC에 부여합니다. (서버에서만 호출)
	 * @param TargetActor 어빌리티를 추가할 캐릭터 (AF1CharacterBase 또는 자식 클래스)
	 */
	UFUNCTION(BlueprintCallable, Category = "F1AbilitySystemLibrary|GAS")
	static void AddCharacterAbilities(AActor* TargetActor);

	// =========================================================================
	// [추가] Gameplay Mechanics (게임플레이 메카닉)
	// =========================================================================

	/**
	 * 몬스터(Victim) 처치 시, 공격자(Killer)에게 경험치와 골드 보상을 지급합니다.
	 * Victim의 CombatInterface를 통해 보상량을 조회하고, RewardGE를 통해 적용합니다.
	 * * @param Killer		보상을 받을 액터 (주로 플레이어)
	 * @param Victim		사망한 액터 (주로 몬스터, 보상 정보를 가짐)
	 * @param RewardGEClass	적용할 보상용 GE 클래스 (GE_KillReward)
	 */
	UFUNCTION(BlueprintCallable, Category = "F1AbilitySystemLibrary|GameplayMechanics")
	static void GiveReward(AActor* Killer, AActor* Victim, TSubclassOf<UGameplayEffect> RewardGEClass);

	// ===========================================
	// GameplayEffect Context Functions
	// ===========================================
	UFUNCTION(BlueprintPure, Category = "F1AbilitySystemLibrary|GameplayEffect")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "F1AbilitySystemLibrary|GameplayEffect")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);
};