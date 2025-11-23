// UI/WidgetController/F1HeroWidgetController.h

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/F1WidgetController.h"
#include "F1HeroWidgetController.generated.h"

// 데이터 변경 시 UI에 쏴줄 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangeSignature, float, NewValue); // 체력, 마나용
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelChangeSignature, int32, NewLevel);     // 레벨용
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNameChangeSignature, const FString&, NewName);// 이름용

/**
 * 영웅 캐릭터(플레이어) 머리 위에 뜨는 체력바 전용 컨트롤러
 * HUD 컨트롤러보다 가볍습니다.
 */
UCLASS(BlueprintType, Blueprintable)
class F1_API UF1HeroWidgetController : public UF1WidgetController
{
	GENERATED_BODY()

public:
	// 초기값 방송 (위젯 생성 직후 1회 호출)
	virtual void BroadcastInitialValues() override;

	// GAS 데이터 변동 감지 (계속 호출)
	virtual void BindCallbacksToDependencies() override;

	// ==============================================================
	// [Delegates] 블루프린트 위젯에서 이 이벤트들을 바인딩해서 씁니다.
	// ==============================================================

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangeSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Status")
	FOnLevelChangeSignature OnLevelChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Status")
	FOnNameChangeSignature OnNameChanged;
};