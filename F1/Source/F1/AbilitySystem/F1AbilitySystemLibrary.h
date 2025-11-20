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

	// ===========================================
	// GameplayEffect Context Functions
	// ===========================================
	UFUNCTION(BlueprintPure, Category = "F1AbilitySystemLibrary|GameplayEffect")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "F1AbilitySystemLibrary|GameplayEffect")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);
};