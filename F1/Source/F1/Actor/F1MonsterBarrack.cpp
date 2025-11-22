// F1MonsterBarrack.cpp

#include "Actor/F1MonsterBarrack.h"
#include "Character/F1MonsterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/OverlapResult.h"

AF1MonsterBarrack::AF1MonsterBarrack()
{
	PrimaryActorTick.bCanEverTick = false;

	// [변경] 1. 빌보드 컴포넌트 (에디터에서 공룡 그림 같은 아이콘으로 보임)
	SpriteComp = CreateDefaultSubobject<UBillboardComponent>("SpriteComp");
	SetRootComponent(SpriteComp);

	// [변경] 2. 범위 확인용 스피어
	SpawnRangeSphere = CreateDefaultSubobject<USphereComponent>("SpawnRangeSphere");
	SpawnRangeSphere->SetupAttachment(RootComponent);
	SpawnRangeSphere->SetSphereRadius(CheckRadius);
	SpawnRangeSphere->SetCollisionProfileName(TEXT("NoCollision"));
}

void AF1MonsterBarrack::BeginPlay()
{
	Super::BeginPlay();

	// 반경 동기화
	SpawnRangeSphere->SetSphereRadius(CheckRadius);

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			SpawnTimerHandle,
			this,
			&AF1MonsterBarrack::SpawnMonster,
			SpawnInterval,
			true
		);
		SpawnMonster();
	}
}

int32 AF1MonsterBarrack::CountMonstersInRadius() const
{
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 배럭 자신은 무시

	// 몬스터는 보통 Pawn 채널을 사용합니다.
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	// 배럭 위치를 중심으로 구체 모양으로 훑습니다.
	bool bHit = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(CheckRadius),
		QueryParams
	);

	int32 MonsterCount = 0;
	if (bHit)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			// 감지된 폰이 내가 소환하려는 몬스터 클래스인지 확인 (플레이어 카운트 방지)
			if (Result.GetActor()->IsA(MonsterClass))
			{
				MonsterCount++;
			}
		}
	}

	return MonsterCount;
}

void AF1MonsterBarrack::SpawnMonster()
{
	if (!MonsterClass) return;

	int32 CurrentCount = CountMonstersInRadius();
	if (CurrentCount >= MaxMonsterCount) return;

	// 1. 레이 트레이스 설정 (배럭 위치에서 아래로 2000cm 쏴봄)
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart - FVector(0.0f, 0.0f, 2000.0f);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // 배럭 자신은 무시

	// 2. 바닥 찾기 (WorldStatic 채널 검사)
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_WorldStatic, // 보통 바닥은 WorldStatic입니다.
		QueryParams
	);

	// 3. 스폰 위치 결정
	FVector SpawnLocation;

	if (bHit)
	{
		// 바닥을 찾았으면: 바닥 위치 + 위로 살짝(100cm) 띄워서 스폰 (발이 땅에 박히지 않게)
		SpawnLocation = HitResult.Location + FVector(0.0f, 0.0f, 100.0f);
	}
	else
	{
		// 바닥을 못 찾았으면 (너무 높거나 구멍): 그냥 배럭 위치에서 생성
		SpawnLocation = GetActorLocation();
	}

	// 4. 회전값 설정 (배럭의 회전값을 따라가거나, 랜덤하게)
	FRotator SpawnRotation = GetActorRotation();
	SpawnRotation.Yaw = FMath::RandRange(0.0f, 360.0f); // 랜덤 방향 보게 하기

	// 5. 스폰
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GetWorld()->SpawnActor<AF1MonsterCharacter>(MonsterClass, SpawnLocation, SpawnRotation, Params);
}