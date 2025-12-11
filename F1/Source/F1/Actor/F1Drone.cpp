#include "Actor/F1Drone.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTag/F1GameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Actor/F1Projectile.h"

AF1Drone::AF1Drone()
{
	PrimaryActorTick.bCanEverTick = true;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 드론은 충돌 무시
}

void AF1Drone::BeginPlay()
{
	Super::BeginPlay();

	// 1초마다 공격 시도
	GetWorldTimerManager().SetTimer(TimerHandle_Attack, this, &AF1Drone::ScanAndAttack, AttackInterval, true);
}

void AF1Drone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FollowOwner(DeltaTime);
}

void AF1Drone::FollowOwner(float DeltaTime)
{
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	// 주인 위치에서 약간 위, 오른쪽 뒤편 목표 설정
	FVector GoalLocation = OwnerActor->GetActorLocation() + FVector(-100.f, 50.f, 150.f);

	// 부드럽게 이동 (VInterpTo)
	FVector NewLocation = FMath::VInterpTo(GetActorLocation(), GoalLocation, DeltaTime, 5.0f);
	SetActorLocation(NewLocation);

	// 회전은 항상 정면 보기 (혹은 타겟 보기)
	SetActorRotation(OwnerActor->GetActorRotation());
}

void AF1Drone::ScanAndAttack()
{
	if (!GetOwner()) return;

	// 1. 주변 스캔 (Sphere Overlap)
	TArray<AActor*> FoundActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // Pawn만 검색

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetActorLocation(),
		ScanRadius,
		ObjectTypes,
		nullptr, // 필터 클래스 (생략 가능)
		{}, // 무시할 액터 (자기 자신 등은 나중에 처리)
		FoundActors
	);

	// 2. 우선순위 타겟 선정
	AActor* BestTarget = FindBestTarget(FoundActors);

	// 3. 공격
	if (BestTarget)
	{
		FireProjectile(BestTarget);
	}
}

AActor* AF1Drone::FindBestTarget(const TArray<AActor*>& OverlappingActors)
{
	AActor* ClosestHero = nullptr;
	AActor* ClosestMonster = nullptr;
	float MinHeroDist = FLT_MAX;
	float MinMonsterDist = FLT_MAX;

	// 태그 싱글톤 (미리 만들어두신거 사용)
	const FF1GameplayTags& GameplayTags = FF1GameplayTags::Get();

	for (AActor* Actor : OverlappingActors)
	{
		// 1. 나 자신이나 주인은 무시
		if (Actor == this || Actor == GetOwner()) continue;

		// 2. 적군인지 확인 (인터페이스나 태그로 확인)
		// 예시: 태그로 적군 판별한다고 가정
		// 실제 프로젝트 로직: IsEnemy(Actor) 함수 활용
		bool bIsEnemy = true; // [TODO] 여기서 팀 체크 로직 넣으세요!
		if (!bIsEnemy) continue;

		// 3. 거리 계산
		float Dist = GetDistanceTo(Actor);

		// 4. 태그 확인 (영웅? 몬스터?)
		FGameplayTagContainer ActorTags;
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);

		if (TargetASC)
		{
			// 2. ASC가 가진 모든 태그를 ActorTags 컨테이너에 복사
			TargetASC->GetOwnedGameplayTags(ActorTags);
		}
		else
		{
			// ASC가 없는 액터(그냥 상자 등)라면 무시
			continue;
		}

		// [핵심] 영웅이면 Hero 후보군에, 아니면 Monster 후보군에 저장
		if (ActorTags.HasTag(GameplayTags.CharacterType_Hero)) // Hero 태그가 있다고 가정
		{
			if (Dist < MinHeroDist)
			{
				MinHeroDist = Dist;
				ClosestHero = Actor;
			}
		}
		else // 몬스터
		{
			if (Dist < MinMonsterDist)
			{
				MinMonsterDist = Dist;
				ClosestMonster = Actor;
			}
		}
	}

	// [결론] 영웅이 한 명이라도 있으면 영웅 리턴, 없으면 몬스터 리턴
	if (ClosestHero) return ClosestHero;
	return ClosestMonster;
}

void AF1Drone::FireProjectile(AActor* Target)
{
	// 1. 주인 정보 가져오기
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);
	if (!OwnerASC) return;

	// 2. 발사 위치 및 회전
	FVector SpawnLocation = GetActorLocation();
	FRotator Rotation = (Target->GetActorLocation() - SpawnLocation).Rotation();
	FTransform SpawnTransform(Rotation, SpawnLocation);

	// 3. 투사체 생성 (Deferred)
	AF1Projectile* Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
		ProjectileClass,
		SpawnTransform,
		OwnerActor,          // Owner: 플레이어
		Cast<APawn>(OwnerActor), // Instigator: 플레이어 (중요: 그래야 킬 판정 받음)
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (Projectile)
	{
		// 4. 데미지 스펙 생성 (주인의 능력치 사용)
		FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
		EffectContext.AddSourceObject(this); // 소스는 드론

		// DamageEffectClass는 헤더에 선언되어 있어야 함 (TSubclassOf<UGameplayEffect>)
		FGameplayEffectSpecHandle SpecHandle = OwnerASC->MakeOutgoingSpec(
			DamageEffectClass,
			1.0f,
			EffectContext
		);

		// 5. 데미지 값 설정 (TODO: 임의로 일단 20데미지)
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
			SpecHandle,
			FF1GameplayTags::Get().DamageType_Physical,
			20.0f
		);

		Projectile->DamageEffectSpecHandle = SpecHandle;

		// 유도탄 설정 (F1Projectile에 SetHomingTarget 구현되어 있다고 가정)
		Projectile->SetHomingTarget(Target);

		// 6. 발사
		Projectile->FinishSpawning(SpawnTransform);
	}
}