// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/F1HeroWidgetController.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/F1WidgetController.h" // 부모 클래스

void UF1HeroWidgetController::BroadcastInitialValues()
{
	// 1. 부모 클래스 로직
	Super::BroadcastInitialValues();

	// 2. 필수 데이터 확인
	if (!GetF1ASC() || !AttributeSet) return;

	// ==============================================================
	// [Part 1] 속성(Attributes) 초기값 방송 (HP, Mana, MaxHP, MaxMana)
	// ==============================================================
	const UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(F1AS->GetHealth());
	OnMaxHealthChanged.Broadcast(F1AS->GetMaxHealth());
	OnManaChanged.Broadcast(F1AS->GetMana());
	OnMaxManaChanged.Broadcast(F1AS->GetMaxMana());

	// 레벨 방송
	OnLevelChanged.Broadcast(static_cast<int32>(F1AS->GetCharacterLevel()));

	// ==============================================================
	// [Part 2] 플레이어 정보 방송 (이름)
	// ==============================================================
	if (PlayerState)
	{
		OnNameChanged.Broadcast(PlayerState->GetPlayerName());
	}

	// [삭제됨] 스킬 아이콘 방송 로직은 OverlayWidgetController로 이사감
}

void UF1HeroWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	// 로그는 필요하면 남겨두고 아니면 삭제
	// UE_LOG(LogTemp, Warning, TEXT("[UI] HeroWidget BindCallbacks Called! IsServer: %d"), GetWorld()->GetNetMode() < NM_Client);

	const UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);
	if (!GetF1ASC()) return;

	// ==============================================================
	// 1. 속성 변경 감지 (HP, Mana, Level)
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
}
