// UI/WidgetController/F1HeroWidgetController.cpp

#include "UI/WidgetController/F1HeroWidgetController.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"

void UF1HeroWidgetController::BroadcastInitialValues()
{
	// 부모 클래스(F1WidgetController)에 저장된 AttributeSet을 가져옴
	const UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);

	// 1. 속성 값 방송 (체력, 마나)
	OnHealthChanged.Broadcast(F1AS->GetHealth());
	OnMaxHealthChanged.Broadcast(F1AS->GetMaxHealth());
	OnManaChanged.Broadcast(F1AS->GetMana());
	OnMaxManaChanged.Broadcast(F1AS->GetMaxMana());

	// 초기 레벨 방송
	OnLevelChanged.Broadcast(static_cast<int32>(F1AS->GetCharacterLevel()));

	// 초기 닉네임 방송 (PlayerState에서 가져옴)
	if (PlayerState)
	{
		OnNameChanged.Broadcast(PlayerState->GetPlayerName());
	}
}

void UF1HeroWidgetController::BindCallbacksToDependencies()
{
	const UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);

	if (!AbilitySystemComponent) return;

	// 1. Health 변경 감지
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetHealthAttribute())
		.AddLambda([this, F1AS](const FOnAttributeChangeData& Data)
			{
				// UI 표기를 위해 0 ~ MaxHealth 사이로 클램핑
				float SafeHealth = FMath::Clamp(Data.NewValue, 0.f, F1AS->GetMaxHealth());
				OnHealthChanged.Broadcast(SafeHealth);
			});

	// 2. MaxHealth 변경 감지
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMaxHealthAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});

	// 3. Mana 변경 감지
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetManaAttribute())
		.AddLambda([this, F1AS](const FOnAttributeChangeData& Data)
			{
				float SafeMana = FMath::Clamp(Data.NewValue, 0.f, F1AS->GetMaxMana());
				OnManaChanged.Broadcast(SafeMana);
			});

	// 4. MaxMana 변경 감지
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMaxManaAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			});

	// 5. 레벨 변경 감지 (선택 사항)
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetCharacterLevelAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				// 어트리뷰트는 float이므로 int32로 캐스팅해서 보냄
				OnLevelChanged.Broadcast(static_cast<int32>(Data.NewValue));
			});
}