// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/F1AbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include <UI/HUD/F1HUD.h>
#include <Game/F1PlayerState.h>
#include "UI/WidgetController/F1WidgetController.h"
#include <F1AbilityTypes.h>
#include <Character/F1CharacterBase.h>
#include "F1AbilitySystemComponent.h"

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

void UF1AbilitySystemLibrary::InitializeDefaultAttributes(AActor* TargetActor)
{
    if (!IsValid(TargetActor) || !TargetActor->HasAuthority()) return;

    AF1CharacterBase* Character = Cast<AF1CharacterBase>(TargetActor);
    UAbilitySystemComponent* ASC = Character ? Character->GetAbilitySystemComponent() : nullptr;

    TSubclassOf<UGameplayEffect> DefaultAttributesGE = Character ? Character->GetDefaultAttributes() : nullptr;

    if (!ASC || !DefaultAttributesGE) return;

    FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
    ContextHandle.AddSourceObject(TargetActor);

    const FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(DefaultAttributesGE, 1.f, ContextHandle);
    ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void UF1AbilitySystemLibrary::AddCharacterAbilities(AActor* TargetActor)
{
	if (!IsValid(TargetActor) || !TargetActor->HasAuthority()) return;

	AF1CharacterBase* Character = Cast<AF1CharacterBase>(TargetActor);
	UF1AbilitySystemComponent* F1ASC = Cast<UF1AbilitySystemComponent>(Character ? Character->GetAbilitySystemComponent() : nullptr);

	if (!F1ASC || !Character) return;

	const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities = Character->GetStartupAbilities();

	if (StartupAbilities.Num() > 0)
	{
		F1ASC->AddCharacterAbilities(StartupAbilities);
	}
}

bool UF1AbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FF1GameplayEffectContext* F1EffectContext = static_cast<const FF1GameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return F1EffectContext->IsCriticalHit();
	}

	return false;
}

void UF1AbilitySystemLibrary::SetIsCriticalHit(UPARAM(ref)FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit)
{
	if (FF1GameplayEffectContext* F1EffectContext = static_cast<FF1GameplayEffectContext*>(EffectContextHandle.Get()))
	{
		F1EffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}
