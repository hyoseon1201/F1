// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/F1OverlayWidgetController.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "Data/F1AbilityInfo.h"
#include "GameplayTagContainer.h"
#include <Character/F1HeroCharacter.h>

void UF1OverlayWidgetController::BroadcastInitialValues()
{
	// 1. 부모(HeroWidgetController)의 방송 (HP, Mana, Level, Name)
	Super::BroadcastInitialValues();

	// 2. 오버레이 전용 스탯 방송
	const UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);

	OnHealthRegenerationChanged.Broadcast(F1AS->GetHealthRegeneration());
	OnManaRegenerationChanged.Broadcast(F1AS->GetManaRegeneration());
	OnAttackDamageChanged.Broadcast(F1AS->GetAttackDamage());
	OnAttackSpeedChanged.Broadcast(F1AS->GetAttackSpeed());
	OnAbilityPowerChanged.Broadcast(F1AS->GetAbilityPower());
	OnCriticalStrikeChanceChanged.Broadcast(F1AS->GetCriticalStrikeChance());
	OnCriticalStrikeDamageChanged.Broadcast(F1AS->GetCriticalStrikeDamage());
	OnArmorChanged.Broadcast(F1AS->GetArmor());
	OnMagicResistanceChanged.Broadcast(F1AS->GetMagicResistance());
	OnMovementSpeedChanged.Broadcast(F1AS->GetMovementSpeed());
	OnAbilityHasteChanged.Broadcast(F1AS->GetAbilityHaste());
	OnArmorPenetrationFlatChanged.Broadcast(F1AS->GetArmorPenetrationFlat());
	OnArmorPenetrationPercentChanged.Broadcast(F1AS->GetArmorPenetrationPercent());
	OnMagicPenetrationFlatChanged.Broadcast(F1AS->GetMagicPenetrationFlat());
	OnMagicPenetrationPercentChanged.Broadcast(F1AS->GetMagicPenetrationPercent());
	OnLifeStealChanged.Broadcast(F1AS->GetLifeSteal());
	OnOmnivampChanged.Broadcast(F1AS->GetOmnivamp());
	OnTenacityChanged.Broadcast(F1AS->GetTenacity());
	OnSlowResistanceChanged.Broadcast(F1AS->GetSlowResistance());
	OnAttackRangeChanged.Broadcast(F1AS->GetAttackRange());
	OnGoldChanged.Broadcast(F1AS->GetGold());
	OnXPChanged.Broadcast(F1AS->GetExperience());
	OnMaxXPChanged.Broadcast(F1AS->GetMaxExperience());

	if (AF1HeroCharacter* Hero = Cast<AF1HeroCharacter>(GetF1ASC()->GetAvatarActor()))
	{
		if (Hero->GetCurrentCharacterInfo().CharacterIcon)
		{
			OnPlayerIconChanged.Broadcast(Hero->GetCurrentCharacterInfo().CharacterIcon);
		}
	}

	// ==============================================================
	// [이사 옴] 스킬(Ability) 아이콘 정보 방송
	// ==============================================================
	if (AbilityInfo)
	{
		GetF1ASC()->ForEachAbility(
			[this](const FGameplayAbilitySpec& AbilitySpec) -> bool
			{
				// InputTag 찾기
				FGameplayTag InputTag = FGameplayTag();
				for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags)
				{
					if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
					{
						InputTag = Tag;
						break;
					}
				}

				if (InputTag.IsValid())
				{
					FGameplayTag AbilityTag = FGameplayTag();
					if (AbilitySpec.Ability->AbilityTags.Num() > 0)
					{
						AbilityTag = AbilitySpec.Ability->AbilityTags.GetByIndex(0);
					}

					// 데이터 에셋에서 정보 찾아서 방송
					FF1AbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag, false);
					if (Info.AbilityTag.IsValid())
					{
						Info.InputTag = InputTag;
						AbilityInfoDelegate.Broadcast(Info);
					}
				}
				return true;
			}
		);
	}
}

void UF1OverlayWidgetController::BindCallbacksToDependencies()
{
	// 1. 부모(HeroWidgetController) 바인딩 (HP, Mana 등)
	Super::BindCallbacksToDependencies();

	const UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);
	if (!GetF1ASC()) return;

	// ==============================================================
	// [기존] 추가 스탯 바인딩
	// ==============================================================
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetHealthRegenerationAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnHealthRegenerationChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetManaRegenerationAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnManaRegenerationChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetAttackDamageAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnAttackDamageChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetAttackSpeedAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnAttackSpeedChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetAbilityPowerAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnAbilityPowerChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetCriticalStrikeChanceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnCriticalStrikeChanceChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetCriticalStrikeDamageAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnCriticalStrikeDamageChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetArmorAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnArmorChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMagicResistanceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMagicResistanceChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMovementSpeedAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMovementSpeedChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetAbilityHasteAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnAbilityHasteChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetArmorPenetrationFlatAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnArmorPenetrationFlatChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetArmorPenetrationPercentAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnArmorPenetrationPercentChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMagicPenetrationFlatAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMagicPenetrationFlatChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMagicPenetrationPercentAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMagicPenetrationPercentChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetLifeStealAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnLifeStealChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetOmnivampAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnOmnivampChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetTenacityAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnTenacityChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetSlowResistanceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnSlowResistanceChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetAttackRangeAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnAttackRangeChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetGoldAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnGoldChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetExperienceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnXPChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMaxExperienceAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxXPChanged.Broadcast(Data.NewValue); }
	);

	// EffectAssetTags 로그용 (유지)
	GetF1ASC()->EffectAssetTags.AddLambda(
		[](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString()));
			}
		}
	);

	// ==============================================================
	// [이사 옴] 쿨타임 태그 변경 감지 (Cooldown Tag)
	// ==============================================================
	if (GetF1ASC())
	{
		if (AbilityInfo)
		{
			for (const FF1AbilityInfo& Info : AbilityInfo->AbilityInformation)
			{
				if (Info.CooldownTag.IsValid())
				{
					// 태그 추가/제거 감지
					GetF1ASC()->RegisterGameplayTagEvent(
						Info.CooldownTag,
						EGameplayTagEventType::NewOrRemoved
					).AddUObject(this, &UF1OverlayWidgetController::OnAbilityTagsChanged);
				}
			}
		}

		// 이펙트 추가 감지 바인딩 (클라이언트 타이밍 이슈 해결용)
		GetF1ASC()->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UF1OverlayWidgetController::OnActiveGameplayEffectAdded);
	}
}

void UF1OverlayWidgetController::OnAbilityTagsChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	// 1. 먼저 쿨타임 태그(CallbackTag)와 매칭되는 InputTag(Q, W, E, R)를 찾습니다.
	FGameplayTag InputTag = FGameplayTag();
	if (AbilityInfo)
	{
		for (const FF1AbilityInfo& Info : AbilityInfo->AbilityInformation)
		{
			// 쿨타임 태그가 일치하면 -> 그 스킬의 InputTag를 가져옴
			if (Info.CooldownTag.MatchesTagExact(CallbackTag))
			{
				InputTag = Info.InputTag;
				break;
			}
		}
	}

	// 2. 쿨타임 시작 (NewCount > 0)
	if (NewCount > 0)
	{
		FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CallbackTag));
		TArray<float> Times = GetF1ASC()->GetActiveEffectsTimeRemaining(Query);

		if (Times.Num() > 0)
		{
			// InputTag를 포함해서 방송!
			OnCooldownChanged.Broadcast(CallbackTag, Times[0], InputTag);
		}
	}
	// 3. 쿨타임 종료
	else
	{
		// 종료될 때도 어떤 키인지 알려주면 좋음 (UI 리셋용)
		OnCooldownChanged.Broadcast(CallbackTag, 0.f, InputTag);
	}
}

void UF1OverlayWidgetController::OnActiveGameplayEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllGrantedTags(AssetTags);

	FGameplayTag CooldownTag = FGameplayTag();
	FGameplayTag InputTag = FGameplayTag(); // [추가] 찾을 변수 준비

	if (AbilityInfo)
	{
		for (const FF1AbilityInfo& Info : AbilityInfo->AbilityInformation)
		{
			// 적용된 이펙트 태그 중에 내 쿨타임 태그가 있는지 확인
			if (AssetTags.HasTagExact(Info.CooldownTag))
			{
				CooldownTag = Info.CooldownTag;
				InputTag = Info.InputTag; // [핵심] 여기서 짝꿍인 InputTag를 찾아냄!
				break;
			}
		}
	}

	// 둘 다 유효할 때만 방송
	if (CooldownTag.IsValid() && InputTag.IsValid())
	{
		FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CooldownTag));
		TArray<float> Times = GetF1ASC()->GetActiveEffectsTimeRemaining(Query);

		if (Times.Num() > 0)
		{
			// UE_LOG(LogTemp, Warning, TEXT("[Overlay] Cooldown Added! Input: %s, Time: %f"), *InputTag.ToString(), Times[0]);

			// InputTag를 포함해서 방송!
			OnCooldownChanged.Broadcast(CooldownTag, Times[0], InputTag);
		}
	}
}