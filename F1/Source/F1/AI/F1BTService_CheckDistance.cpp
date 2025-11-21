// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/F1BTService_CheckDistance.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Pawn.h"

UF1BTService_CheckDistance::UF1BTService_CheckDistance()
{
	NodeName = TEXT("Check Distance to Home");
	Interval = 0.5f;        // 0.5초마다 검사 (성능 조절 가능)
	RandomDeviation = 0.1f; // 틱 타이밍을 살짝 섞음
}

void UF1BTService_CheckDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 1. 필수 컴포넌트 가져오기
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!ControllingPawn || !Blackboard) return;

	// 2. 거리 계산
	FVector CurrentLoc = ControllingPawn->GetActorLocation();
	FVector HomeLoc = Blackboard->GetValueAsVector(HomeLocationKey.SelectedKeyName);

	// 높이 차이는 무시하고 수평 거리만 잴 경우 Dist2D 사용, 전체는 Dist
	float Distance = FVector::Dist(CurrentLoc, HomeLoc);

	// 3. 현재 상태 확인 (이미 복귀 중인가?)
	bool bIsReturning = Blackboard->GetValueAsBool(IsReturningKey.SelectedKeyName);

	if (bIsReturning)
	{
		// [복귀 중일 때 로직]
		// 집에 거의 도착했는가?
		if (Distance < ReturnAcceptanceRadius)
		{
			// 도착했으면 복귀 모드 해제 -> 다시 대기/전투 가능
			Blackboard->SetValueAsBool(IsReturningKey.SelectedKeyName, false);

			// (선택) 도착하면 체력을 회복시킨다거나 하는 로직 추가 가능
		}
	}
	else
	{
		// [일반 상태(전투/대기)일 때 로직]
		// 집에서 너무 멀어졌는가?
		if (Distance > MaxLeashRange)
		{
			// 복귀 모드 활성화
			Blackboard->SetValueAsBool(IsReturningKey.SelectedKeyName, true);

			// **중요** 타겟을 지워버려야 몬스터가 플레이어를 안 쫓아감
			Blackboard->ClearValue(TargetActorKey.SelectedKeyName);

			// (선택) 몬스터에게 스피드 버프를 걸거나 무적을 걸 수도 있음
		}
	}
}