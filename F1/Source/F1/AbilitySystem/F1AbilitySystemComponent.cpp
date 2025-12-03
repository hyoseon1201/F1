// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "AbilitySystem/GameplayAbility/F1GameplayAbility.h"

void UF1AbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UF1AbilitySystemComponent::ClientEffectApplied);
}

void UF1AbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (const UF1GameplayAbility* F1Ability = Cast<UF1GameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(F1Ability->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

void UF1AbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UF1AbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive()) TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void UF1AbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)) AbilitySpecInputReleased(AbilitySpec);
	}
}

void UF1AbilitySystemComponent::ForEachAbility(const TFunctionRef<bool(const FGameplayAbilitySpec&)>& Func)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		// Delegate.ExecuteIfBound 대신 그냥 함수처럼 호출
		if (Func(AbilitySpec))
		{
			// ...
		}
	}
}

// [핵심 추가] 헬퍼 함수 구현
TArray<float> UF1AbilitySystemComponent::GetActiveEffectsTimeRemaining(const FGameplayEffectQuery& Query) const
{
	TArray<float> Times;

	// 쿼리에 맞는 활성 이펙트 핸들 가져오기
	TArray<FActiveGameplayEffectHandle> ActiveEffects = GetActiveEffects(Query);

	for (const FActiveGameplayEffectHandle& Handle : ActiveEffects)
	{
		const FActiveGameplayEffect* Effect = GetActiveGameplayEffect(Handle);
		if (Effect)
		{
			// 현재 시간 기준으로 남은 시간 계산
			float Remaining = Effect->GetTimeRemaining(GetWorld()->GetTimeSeconds());
			Times.Add(Remaining);
		}
	}
	return Times;
}

void UF1AbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}