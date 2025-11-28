#include "UI/Widget/F1AbilityGauge.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UF1AbilityGauge::SetAbilityInfo(const FF1AbilityInfo& Info)
{
	// 1. 태그 저장 (나중에 쿨타임 이벤트 왔을 때 내 건지 확인용)
	AbilityTag = Info.AbilityTag;
	CooldownTag = Info.CooldownTag;

	// 2. 아이콘 이미지 변경
	if (Icon && Info.Icon)
	{
		Icon->SetBrushFromTexture(Info.Icon);
	}

	// (선택) 초기화 시 쿨타임 텍스트/이미지 숨기기
	if (CooldownCounterText) CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	if (CooldownImage) CooldownImage->SetVisibility(ESlateVisibility::Hidden);
}