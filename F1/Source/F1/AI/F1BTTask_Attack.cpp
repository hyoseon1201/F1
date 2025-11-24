
#include "AI/F1BTTask_Attack.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Interaction/F1CombatInterface.h"

UF1BTTask_Attack::UF1BTTask_Attack()
{
	NodeName = TEXT("Attack"); // BT 에디터에 표시될 이름
}

EBTNodeResult::Type UF1BTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 1. AI 컨트롤러를 통해 제어 중인 폰(Pawn)을 가져옴
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		return EBTNodeResult::Failed;
	}

	// 2. 폰이 전투 인터페이스를 가지고 있는지 확인 (Cast)
	IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(ControllingPawn);

	if (CombatInterface)
	{
		// 3. 공격 함수 호출! (몬스터의 오버라이드된 함수가 실행됨)
		CombatInterface->Attack();

		// 4. 성공 반환 -> BT는 다음 노드(Wait 등)로 넘어감
		return EBTNodeResult::Succeeded;
	}

	// 인터페이스가 없으면 실패 처리
	return EBTNodeResult::Failed;
}