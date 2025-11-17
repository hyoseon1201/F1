
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "F1AbilitySystemGlobals.generated.h"

/**
 *
 */
UCLASS()
class F1_API UF1AbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};