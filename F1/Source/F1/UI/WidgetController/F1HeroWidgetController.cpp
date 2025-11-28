// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/F1HeroWidgetController.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "Data/F1AbilityInfo.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTagContainer.h"

void UF1HeroWidgetController::BroadcastInitialValues()
{
	// 1. 부모 클래스 로직 (안전빵)
	Super::BroadcastInitialValues();

	// 2. 필수 데이터 확인
	if (!GetF1ASC() || !AttributeSet) return;

	// ==============================================================
	// [Part 1] 속성(Attributes) 초기값 방송
	// ==============================================================
	const UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(F1AS->GetHealth());
	OnMaxHealthChanged.Broadcast(F1AS->GetMaxHealth());
	OnManaChanged.Broadcast(F1AS->GetMana());
	OnMaxManaChanged.Broadcast(F1AS->GetMaxMana());

	// 레벨 방송
	OnLevelChanged.Broadcast(static_cast<int32>(F1AS->GetCharacterLevel()));

	// ==============================================================
	// [Part 2] 플레이어 정보 방송
	// ==============================================================
	if (PlayerState)
	{
		OnNameChanged.Broadcast(PlayerState->GetPlayerName());
	}

	// ==============================================================
	// [Part 3] 스킬(Ability) 아이콘 정보 방송
	// ==============================================================
	if (AbilityInfo)
	{
		// TFunctionRef 방식을 사용하거나, 델리게이트 타입에 맞춰 람다 전달
		GetF1ASC()->ForEachAbility(
			[this](const FGameplayAbilitySpec& AbilitySpec) -> bool
			{
				// 3-1. InputTag 찾기
				FGameplayTag InputTag = FGameplayTag();
				for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags)
				{
					if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
					{
						InputTag = Tag;
						break;
					}
				}

				// 3-2. InputTag가 있으면 UI에 등록
				if (InputTag.IsValid())
				{
					FGameplayTag AbilityTag = FGameplayTag();
					if (AbilitySpec.Ability->AbilityTags.Num() > 0)
					{
						AbilityTag = AbilitySpec.Ability->AbilityTags.GetByIndex(0);
					}

					// 데이터 에셋에서 아이콘 등 정보 찾기
					FF1AbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
					Info.InputTag = InputTag; // 현재 할당된 키 정보 덮어쓰기

					// UI로 방송
					AbilityInfoDelegate.Broadcast(Info);
				}

				return true;
			}
		);
	}
}

void UF1HeroWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	const UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);
	if (!GetF1ASC()) return;

	// ==============================================================
	// 1. 속성 변경 감지 (람다)
	// ==============================================================
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetHealthAttribute()).AddLambda(
		[this, F1AS](const FOnAttributeChangeData& Data) {
			OnHealthChanged.Broadcast(FMath::Clamp(Data.NewValue, 0.f, F1AS->GetMaxHealth()));
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetManaAttribute()).AddLambda(
		[this, F1AS](const FOnAttributeChangeData& Data) {
			OnManaChanged.Broadcast(FMath::Clamp(Data.NewValue, 0.f, F1AS->GetMaxMana()));
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxManaChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetCharacterLevelAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnLevelChanged.Broadcast(static_cast<int32>(Data.NewValue)); }
	);

	// ==============================================================
	// 2. 쿨타임 태그 변경 감지 (Cooldown Tag)
	// ==============================================================
	if (AbilityInfo)
	{
		for (const FF1AbilityInfo& Info : AbilityInfo->AbilityInformation)
		{
			if (Info.CooldownTag.IsValid())
			{
				// 해당 쿨타임 태그가 추가되거나(쿨 시작) 제거될 때(쿨 끝) 알림 받기
				GetF1ASC()->RegisterGameplayTagEvent(
					Info.CooldownTag,
					EGameplayTagEventType::NewOrRemoved
				).AddUObject(this, &UF1HeroWidgetController::OnAbilityTagsChanged);
			}
		}
	}
}

void UF1HeroWidgetController::OnAbilityTagsChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	// 쿨타임 태그가 붙음 (NewCount > 0) -> 쿨타임 시작
	// 쿨타임 태그가 떨어짐 (NewCount == 0) -> 쿨타임 종료

	if (NewCount > 0)
	{
		// 해당 태그를 가진 활성 이펙트(GE_Cooldown)의 남은 시간을 찾음
		FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CallbackTag));

		// ASC에 추가해둔 헬퍼 함수 사용
		TArray<float> Times = GetF1ASC()->GetActiveEffectsTimeRemaining(Query);

		if (Times.Num() > 0)
		{
			// 가장 긴 시간을 보냄 (보통 하나뿐임)
			float TimeRemaining = Times[0];
			OnCooldownChanged.Broadcast(CallbackTag, TimeRemaining);
		}
	}
	else
	{
		// 쿨타임 끝남 -> 0초 전송 (UI 끄기용)
		OnCooldownChanged.Broadcast(CallbackTag, 0.f);
	}
}