// F1MoveToTarget.cpp

#include "F1MoveToTarget.h" // 헤더 경로 주의
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h" // 캐릭터 캐스팅용
#include "Blueprint/AIBlueprintHelperLibrary.h"

UF1MoveToTarget::UF1MoveToTarget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
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

	// 1. 유효성 검사
	bool bHasActor = (TargetActor != nullptr);
	bool bHasLocation = !TargetLocation.IsZero();
	if (!bHasActor && !bHasLocation)
	{
		if (ShouldBroadcastAbilityTaskDelegates()) OnCancelled.Broadcast();
		EndTask();
		return;
	}

	// 2. 서버/클라이언트 모두 이동 명령 실행
	AActor* Avatar = GetAvatarActor();
	if (Avatar)
	{
		APawn* Pawn = Cast<APawn>(Avatar);
		if (Pawn && Pawn->GetController())
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] Trying to Move..."), (Avatar->GetNetMode() == NM_Client ? TEXT("Client") : TEXT("Server")));

			if (TargetActor)
				UAIBlueprintHelperLibrary::SimpleMoveToActor(Pawn->GetController(), TargetActor);
			else
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(Pawn->GetController(), TargetLocation);
		}
	}

	// 3. 거리 체크 시작
	CheckDistanceAndFinish();
}

void UF1MoveToTarget::TickTask(float DeltaTime)
{
	if (bIsFinished) return;
	Super::TickTask(DeltaTime);

	// 클라이언트/서버 모두 매 프레임 거리를 잽니다.
	// 서버는 이동 중이라 거리가 줄어들 것이고,
	// 클라이언트는 리플리케이션된 위치를 받아 거리가 줄어드는 것을 감지합니다.
	CheckDistanceAndFinish();
}

void UF1MoveToTarget::PerformFaceTarget()
{
	ACharacter* AvatarChar = Cast<ACharacter>(GetAvatarActor());
	if (!AvatarChar) return;

	FVector Destination = (TargetActor) ? TargetActor->GetActorLocation() : TargetLocation;
	FVector Dir = (Destination - AvatarChar->GetActorLocation()).GetSafeNormal2D();

	if (!Dir.IsZero())
	{
		AvatarChar->SetActorRotation(Dir.Rotation());
	}
}

void UF1MoveToTarget::OnDestroy(bool bInOwnerFinished)
{
	if (!bIsFinished) // 성공해서 끝난 게 아니라, 취소되거나 강제 종료된 경우
	{
		if (AActor* Avatar = GetAvatarActor())
		{
			if (APawn* Pawn = Cast<APawn>(Avatar))
			{
				if (AController* Controller = Pawn->GetController())
					Controller->StopMovement();
			}
		}
	}
	Super::OnDestroy(bInOwnerFinished);
}

void UF1MoveToTarget::CheckDistanceAndFinish()
{
	AActor* Avatar = GetAvatarActor();
	if (!Avatar) return;

	FVector Destination = TargetLocation;
	if (TargetActor) Destination = TargetActor->GetActorLocation();

	// 2D 거리 계산 (높이차 무시)
	float Dist = FVector::Dist2D(Avatar->GetActorLocation(), Destination);

	// [중요] 타겟의 캡슐 반경 고려 (선택 사항)
	// 만약 몬스터에게 다가가는 거라면 몬스터 몸집만큼 덜 가도 도착으로 쳐야 합니다.
	// if (TargetActor) Dist -= TargetActor->GetSimpleCollisionRadius();

	// 도착 판정
	if (Dist <= StopDistance)
	{
		// 양쪽 모두 이동 정지 명령
		APawn* Pawn = Cast<APawn>(Avatar);
		if (Pawn && Pawn->GetController())
		{
			Pawn->GetController()->StopMovement();
		}

		PerformFaceTarget();

		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnReached.Broadcast();
		}
		bIsFinished = true;
		EndTask();
	}
}