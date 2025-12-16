#include "F1GATA_TraceMouseCursor.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/PlayerController.h"

AF1GATA_TraceMouseCursor::AF1GATA_TraceMouseCursor()
{
	// 틱(Tick)을 돌며 계속 추적할 필요 없이, 클릭 순간만 가져오면 되므로 false
	PrimaryActorTick.bCanEverTick = true;
	ShouldProduceTargetDataOnServer = false;
}

void AF1GATA_TraceMouseCursor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	ConfirmTargetingAndContinue();
}

void AF1GATA_TraceMouseCursor::ConfirmTargetingAndContinue()
{
	APlayerController* PC = OwningAbility->GetCurrentActorInfo()->PlayerController.Get();
	if (!PC)
	{
		// PC가 없으면(AI 등) 그냥 취소
		TargetDataReadyDelegate.Broadcast(FGameplayAbilityTargetDataHandle());
		return;
	}

	FGameplayAbilityTargetDataHandle Handle;

	// 1. 마우스 위치 가져오기
	FHitResult HitResult;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	// 2. 데이터 포장
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
	Handle.Add(TargetData);

	// 3. 데이터 전송 (GAS 내부적으로 서버로 보냄)
	TargetDataReadyDelegate.Broadcast(Handle);
}