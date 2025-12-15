#include "Actor/F1MeteorCircle.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/F1GameplayTags.h"
#include "Character/F1CharacterBase.h"
#include "Interaction/F1CombatInterface.h" // 인터페이스 헤더 필수
#include "DrawDebugHelpers.h" // 디버깅용
#include "GameFramework/PlayerState.h"

AF1MeteorCircle::AF1MeteorCircle()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // [중요] 장판은 서버->클라 복제되어야 함

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// [수정 권장] Pawn뿐만 아니라 PhysicsBody 등도 감지하려면 Profile을 쓰는 게 좋습니다.
	// 일단 Pawn은 확실히 감지하도록 설정
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap); // 혹시 적이 Dynamic일 경우 대비

	MeteorCircleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeteorCircleMesh"));
	MeteorCircleMesh->SetupAttachment(RootComponent);
	MeteorCircleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeteorCircleMesh->SetCastShadow(false);
	MeteorCircleMesh->SetRelativeLocation(FVector(0.f, 0.f, -1.0f));
}

void AF1MeteorCircle::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->SetSphereRadius(DamageRadius);

	// [DEBUG] 시작 로그
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Meteor] Spawned on SERVER. Radius: %f, Period: %f"), DamageRadius, DamagePeriod);
	}

	// 타이머 시작
	GetWorldTimerManager().SetTimer(TimerHandle_Damage, this, &AF1MeteorCircle::ApplyPeriodicDamage, DamagePeriod, true);

	// 첫 틱 즉시 실행 (선택 사항: 장판 깔리자마자 데미지 줄지 여부)
	// ApplyPeriodicDamage(); 

	SetLifeSpan(5.0f);
}

void AF1MeteorCircle::ApplyPeriodicDamage()
{
	// 1. [서버 권한 체크]
	if (!HasAuthority()) return;

	// 2. [주인 확인]
	AActor* Caster = GetInstigator();
	if (!Caster) Caster = GetOwner();

	if (!Caster)
	{
		UE_LOG(LogTemp, Error, TEXT("[Meteor] FAILED: Caster is NULL!"));
		return;
	}

	// ===========================================================================
	// [핵심 수정] Caster가 'PlayerState'이거나 'Controller'인 경우 -> 실제 'Pawn' 찾기
	// ===========================================================================

	// A. Caster가 PlayerState인 경우 (GAS 구조상 흔함)
	if (APlayerState* PS = Cast<APlayerState>(Caster))
	{
		Caster = PS->GetPawn();
		// [DEBUG] 변환 확인
		// UE_LOG(LogTemp, Log, TEXT("[Meteor] Converted PlayerState to Pawn: %s"), *GetNameSafe(Caster));
	}
	// B. Caster가 Controller인 경우
	else if (APlayerController* PC = Cast<APlayerController>(Caster))
	{
		Caster = PC->GetPawn();
	}

	// Pawn을 못 찾았으면 중단 (로그아웃 등)
	if (!Caster) return;

	// ---------------------------------------------------------------------------

	// 시전자의 인터페이스 가져오기
	IF1CombatInterface* CasterCombat = Cast<IF1CombatInterface>(Caster);
	int32 CasterTeamID = -1;

	if (CasterCombat)
	{
		CasterTeamID = CasterCombat->GetTeamID();
	}

	// 3. 범위 감지
	TSet<AActor*> OverlappingActors;
	SphereComp->GetOverlappingActors(OverlappingActors);

	// (디버그 구체 끄고 싶으면 주석 처리)
	// DrawDebugSphere(GetWorld(), GetActorLocation(), DamageRadius, 12, FColor::Red, false, 0.5f);

	for (AActor* TargetActor : OverlappingActors)
	{
		if (!TargetActor || TargetActor == Caster) continue;

		IF1CombatInterface* TargetCombat = Cast<IF1CombatInterface>(TargetActor);

		// 4. [팀 구분 로직]
		if (CasterCombat && TargetCombat)
		{
			if (CasterCombat->GetTeamID() == TargetCombat->GetTeamID())
			{
				continue; // 아군이므로 패스
			}
		}

		// 5. 데미지 적용
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (TargetASC && DamageEffectSpecHandle.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
			// UE_LOG(LogTemp, Warning, TEXT("[Meteor] HIT! Applied Damage to: %s"), *GetNameSafe(TargetActor));
		}
	}
}