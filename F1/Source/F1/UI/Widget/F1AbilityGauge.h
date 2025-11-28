#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/F1UserWidget.h"
#include "Data/F1AbilityInfo.h"
#include "GameplayTagContainer.h"
#include "F1AbilityGauge.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class F1_API UF1AbilityGauge : public UF1UserWidget
{
	GENERATED_BODY()

public:
	// [1] 초기화: 아이콘과 태그 정보를 받아서 세팅
	UFUNCTION(BlueprintCallable, Category = "GAS|UI")
	void SetAbilityInfo(const FF1AbilityInfo& Info);

	// [2] 쿨타임 발동 알림: 컨트롤러가 호출해주는 이벤트
	// (C++ 구현부 없음! 블루프린트에서 시각 효과 구현)
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "GAS|UI")
	void PlayCooldownAnimation(FGameplayTag InAbilityTag, float InCooldownTime);

protected:
	// 내가 담당하는 스킬 태그 (예: Ability.Skill.Q)
	UPROPERTY(BlueprintReadOnly, Category = "GAS|UI")
	FGameplayTag AbilityTag;

	// 쿨타임 태그 (예: Cooldown.Skill.Q) - 나중에 비교용
	UPROPERTY(BlueprintReadOnly, Category = "GAS|UI")
	FGameplayTag CooldownTag;

	// =================================================
	// 위젯 바인딩 (이름을 BP와 일치시켜야 함)
	// =================================================
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> Icon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> CooldownCounterText; // 남은 시간 숫자 (5, 4, 3...)

	// 쿨타임 돌 때 어둡게 덮을 이미지 (보통 Image를 씁니다)
	// 머티리얼을 써서 부채꼴로 깎으려면 Image가 필요합니다.
	// TextBlock이 아니라 Image로 변경하는 것을 추천합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> CooldownImage;
};