#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "F1GATA_TraceMouseCursor.generated.h"

/**
 * 마우스 커서 위치의 바닥 좌표를 가져오는 타겟 액터
 */
UCLASS()
class AF1GATA_TraceMouseCursor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AF1GATA_TraceMouseCursor();

	// 이 함수가 핵심입니다. 타겟팅을 시작하라는 명령.
	virtual void StartTargeting(UGameplayAbility* Ability) override;

	// 타겟 데이터를 확정하고 전송하는 함수
	virtual void ConfirmTargetingAndContinue() override;
};