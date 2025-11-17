#include "AbilitySystem/F1AbilitySystemGlobals.h"

#include "F1AbilityTypes.h"

FGameplayEffectContext* UF1AbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FF1GameplayEffectContext();
}

