
#include "Actor/F1MeteorCircle.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayTag/F1GameplayTags.h"
#include <Character/F1CharacterBase.h>

AF1MeteorCircle::AF1MeteorCircle()
{
	PrimaryActorTick.bCanEverTick = false;

	// 1. 범위 컴포넌트 (루트)
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 2. [변경] 마법진 메시
	MeteorCircleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeteorCircleMesh"));
	MeteorCircleMesh->SetupAttachment(RootComponent);

	// [핵심 설정 3대장]
	// 1) 충돌 끔: 장판은 밟고 지나가야 함
	MeteorCircleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 2) 그림자 끔: 마법진은 보통 스스로 빛나므로 그림자가 지면 이상함
	MeteorCircleMesh->SetCastShadow(false);

	// 3) 위치 조절: 구체 중심보다 살짝 아래(바닥)에 깔리게
	// (메시의 피벗이 중앙이라면 -Z축으로 좀 내려야 바닥에 붙습니다)
	MeteorCircleMesh->SetRelativeLocation(FVector(0.f, 0.f, -1.0f));
}

void AF1MeteorCircle::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->SetSphereRadius(DamageRadius);

	// [팁] 데미지 범위(Radius)에 맞춰서 메시 크기 키우기
	// 메시의 기본 크기가 100단위(1m)라고 가정할 때의 공식입니다.
	// 만약 메시가 너무 크거나 작으면 블루프린트에서 Scale을 직접 조절하세요.
	// float Scale = DamageRadius / 50.0f; // 적당한 비율 계산
	// MagicCircleMesh->SetWorldScale3D(FVector(Scale, Scale, 1.0f));

	GetWorldTimerManager().SetTimer(TimerHandle_Damage, this, &AF1MeteorCircle::ApplyPeriodicDamage, DamagePeriod, true);

	SetLifeSpan(5.0f);
}

void AF1MeteorCircle::ApplyPeriodicDamage()
{
	// 0. 시전자(Caster) 정보 가져오기 (주인이 없으면 데미지 판정 불가)
	AActor* Caster = GetInstigator();
	if (!Caster) return;

	// 시전자의 인터페이스 가져오기 (팀 확인용)
	IF1CombatInterface* CasterCombat = Cast<IF1CombatInterface>(Caster);

	// 1. 범위 안에 있는 액터들 찾기
	TSet<AActor*> OverlappingActors;
	SphereComp->GetOverlappingActors(OverlappingActors);

	for (AActor* TargetActor : OverlappingActors)
	{
		// 유효성 검사 및 자해 방지 (혹시 모를 안전장치)
		if (!TargetActor || TargetActor == Caster) continue;

		// 2. [팀 구분 핵심 로직] 
		IF1CombatInterface* TargetCombat = Cast<IF1CombatInterface>(TargetActor);

		// 둘 다 인터페이스가 있고, 팀이 같다면? -> 아군이므로 패스(continue)
		if (CasterCombat && TargetCombat)
		{
			if (CasterCombat->GetTeamID() == TargetCombat->GetTeamID())
			{
				continue; // 데미지 주지 않고 다음 타겟으로 넘어감
			}
		}

		// 3. 데미지 적용
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (TargetASC && DamageEffectSpecHandle.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	}
}