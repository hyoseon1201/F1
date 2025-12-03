// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/F1AbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include <UI/HUD/F1HUD.h>
#include <Game/F1PlayerState.h>
#include "UI/WidgetController/F1WidgetController.h"
#include <F1AbilityTypes.h>
#include <Character/F1CharacterBase.h>
#include "F1AbilitySystemComponent.h"
#include <AbilitySystemBlueprintLibrary.h>
#include <GameplayTag/F1GameplayTags.h>

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

void UF1AbilitySystemLibrary::GiveReward(AActor* Killer, AActor* Victim, TSubclassOf<UGameplayEffect> RewardGEClass)
{
    UE_LOG(LogTemp, Warning, TEXT("[GiveReward] Called. Killer: %s, Victim: %s"), *GetNameSafe(Killer), *GetNameSafe(Victim));
    if (!Killer || !Victim || !RewardGEClass)
    {
        UE_LOG(LogTemp, Error, TEXT("[GiveReward] FAILED: Missing Actor or GE Class"));
        return;
    }

    // 1. 죽은 놈(Victim)에게서 보상 정보(XP, Gold)를 뜯어냅니다.
    // (인터페이스 함수 호출)
    int32 XP = 0;
    int32 Gold = 0;

    if (Victim->Implements<UF1CombatInterface>())
    {
        XP = IF1CombatInterface::Execute_GetRewardXP(Victim);
        Gold = IF1CombatInterface::Execute_GetRewardGold(Victim);
    }

    UE_LOG(LogTemp, Warning, TEXT("[GiveReward] Amount -> XP: %d, Gold: %d"), XP, Gold);
    // 2. 죽인 놈(Killer)의 ASC를 가져옵니다.
    UAbilitySystemComponent* KillerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Killer);
    if (!KillerASC) return;

    // 3. 보상용 GE(GE_KillReward)를 만들어서 적용합니다.
    FGameplayEffectContextHandle ContextHandle = KillerASC->MakeEffectContext();
    ContextHandle.AddSourceObject(Killer);

    const FGameplayEffectSpecHandle SpecHandle = KillerASC->MakeOutgoingSpec(RewardGEClass, 1.0f, ContextHandle);

    if (SpecHandle.Data.IsValid())
    {
        const FF1GameplayTags& GameplayTags = FF1GameplayTags::Get();

        // SetByCaller로 값 주입 (아까 만든 태그 사용)
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Meta_Experience, XP);
        UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Attributes_Meta_Gold, Gold);

        UE_LOG(LogTemp, Warning, TEXT("[GiveReward] Applying GE to Killer..."));
        // 4. 쏴라!
        KillerASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
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
