// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AbilityTask/F1MoveToTarget.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UF1MoveToTarget::UF1MoveToTarget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true; // Tick 활성화
	bIsFinished = false;
}

UF1MoveToTarget* UF1MoveToTarget::MoveToTarget(UGameplayAbility* OwningAbility, AActor* TargetActor, FVector TargetLocation, float StopDistance)
{
	UF1MoveToTarget* MyTask = NewAbilityTask<UF1MoveToTarget>(OwningAbility);

	MyTask->TargetActor = TargetActor;
	MyTask->TargetLocation = TargetLocation;
	MyTask->StopDistance = StopDistance;

	return MyTask;
}

void UF1MoveToTarget::Activate()
{
	Super::Activate();

	// [수정] 유효성 검사 강화
	// 액터도 없고, 위치도 (0,0,0)이면 이동할 곳이 없으므로 취소
	bool bHasActor = (TargetActor != nullptr);
	bool bHasLocation = !TargetLocation.IsZero();

	if (!bHasActor && !bHasLocation)
	{
		// 실패 로그 (필요시 주석 해제)
		// UE_LOG(LogTemp, Warning, TEXT("MoveToTarget Failed: No Target Actor and Location is Zero."));

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast();
		}
		EndTask();
	}
}

void UF1MoveToTarget::TickTask(float DeltaTime)
{
	if (bIsFinished) return;

	Super::TickTask(DeltaTime);

	AActor* Avatar = GetAvatarActor();
	if (!Avatar)
	{
		EndTask();
		return;
	}

	APawn* Pawn = Cast<APawn>(Avatar);
	if (!Pawn) return;
	AController* Controller = Pawn->GetController();
	if (!Controller) return;

	// 1. 목표 지점 결정 (액터가 있으면 액터 위치, 없으면 고정 위치)
	FVector Destination = TargetLocation;
	if (TargetActor)
	{
		Destination = TargetActor->GetActorLocation();
	}

	// 2. 거리 계산 (2D 거리 사용 권장)
	float Dist = FVector::Dist2D(Avatar->GetActorLocation(), Destination);

	// 3. 도착 판정
	if (Dist <= StopDistance)
	{
		// 도착함! 이동 멈춤
		Controller->StopMovement();

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnReached.Broadcast(); // 성공 신호 발송
		}
		bIsFinished = true;
		EndTask();
	}
	else
	{
		// 4. 이동 명령 (아직 멀었음)
		if (TargetActor)
		{
			// 액터 추적 (움직이는 대상)
			UAIBlueprintHelperLibrary::SimpleMoveToActor(Controller, TargetActor);
		}
		else
		{
			// 좌표 이동 (고정된 대상)
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, Destination);
		}
	}
}

void UF1MoveToTarget::OnDestroy(bool bInOwnerFinished)
{
	// 태스크가 끝날 때(성공, 취소, 중단 등) 이동을 멈추게 함
	if (AActor* Avatar = GetAvatarActor())
	{
		if (APawn* Pawn = Cast<APawn>(Avatar))
		{
			if (AController* Controller = Pawn->GetController())
			{
				Controller->StopMovement();
			}
		}
	}

	Super::OnDestroy(bInOwnerFinished);
}