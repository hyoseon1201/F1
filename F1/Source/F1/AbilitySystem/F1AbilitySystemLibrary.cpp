// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/F1AbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include <UI/HUD/F1HUD.h>
#include <Game/F1PlayerState.h>
#include "UI/WidgetController/F1WidgetController.h"
#include <F1AbilityTypes.h>

UF1OverlayWidgetController* UF1AbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AF1HUD* F1HUD = Cast<AF1HUD>(PC->GetHUD()))
		{
			AF1PlayerState* PS = PC->GetPlayerState<AF1PlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return F1HUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}

	return nullptr;
}

bool UF1AbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FF1GameplayEffectContext* F1EffectContext = static_cast<const FF1GameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return F1EffectContext->IsCriticalHit();
	}

	return false;
}

void UF1AbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FF1GameplayEffectContext* F1EffectContext = static_cast<FF1GameplayEffectContext*>(EffectContextHandle.Get()))
	{
		F1EffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}
