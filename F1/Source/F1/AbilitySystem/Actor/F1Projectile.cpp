#include "AbilitySystem/Actor/F1Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Components/AudioComponent.h"
#include "F1.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "Engine/OverlapResult.h"
#include "Character/F1CharacterBase.h"

AF1Projectile::AF1Projectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionProfileName(FName("Projectile"));

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AF1Projectile::SetHomingTarget(AActor* TargetActor)
{
	if (TargetActor && ProjectileMovement)
	{
		ProjectileMovement->bIsHomingProjectile = true;
		ProjectileMovement->HomingAccelerationMagnitude = 20000.f;
		ProjectileMovement->HomingTargetComponent = TargetActor->GetRootComponent();
	}
}

void AF1Projectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);

	if (Sphere)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AF1Projectile::OnSphereOverlap);
		Sphere->OnComponentHit.AddDynamic(this, &AF1Projectile::OnHit);
	}
}

void AF1Projectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		if (ImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, GetActorLocation());
		}
	}
	Super::Destroyed();
}

// ============================================================================
// [DEBUG] OnSphereOverlap (직격)
// ============================================================================
void AF1Projectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return; // 서버만 처리

	AActor* ProjectileOwner = GetOwner();
	if (!ProjectileOwner)
	{
		// [DEBUG 1] 주인이 없는 경우 (RPC 과정에서 SetOwner 누락 가능성)
		UE_LOG(LogTemp, Error, TEXT("[Projectile] FAILED: Projectile has no Owner! OtherActor: %s"), *GetNameSafe(OtherActor));
		Destroy();
		return;
	}

	if (OtherActor == ProjectileOwner) return;
	if (bHit) return;

	// [DEBUG 2] 충돌 감지 성공
	UE_LOG(LogTemp, Log, TEXT("[Projectile] Overlap Detected with: %s"), *GetNameSafe(OtherActor));

	IF1CombatInterface* SourceCombat = Cast<IF1CombatInterface>(ProjectileOwner);
	IF1CombatInterface* TargetCombat = Cast<IF1CombatInterface>(OtherActor);

	if (SourceCombat && TargetCombat)
	{
		int32 MyTeam = SourceCombat->GetTeamID();
		int32 EnemyTeam = TargetCombat->GetTeamID();

		// [DEBUG] 여기서 로그가 찍히는지 보세요!
		UE_LOG(LogTemp, Warning, TEXT("[Projectile] Hit Check: MyTeam(%d) vs EnemyTeam(%d)"), MyTeam, EnemyTeam);

		if (MyTeam == EnemyTeam)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Projectile] Friendly Fire! Ignored."));
			return; // 같은 팀(0 vs 0)이면 여기서 막혀서 ExecCalc가 안 불립니다.
		}
	}

	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, GetActorLocation());
	}

	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		if (DamageEffectSpecHandle.Data.IsValid())
		{
			// [DEBUG 4] 데미지 적용 시도
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
			UE_LOG(LogTemp, Warning, TEXT("[Projectile] SUCCESS: ApplyGameplayEffect to %s"), *GetNameSafe(OtherActor));
		}
		else
		{
			// [DEBUG 5] SpecHandle이 비어있음 (가장 유력한 원인)
			UE_LOG(LogTemp, Error, TEXT("[Projectile] FAILED: DamageEffectSpecHandle is INVALID! Check BP_ProjectileSpell Activate logic."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Projectile] Target has no ASC."));
	}

	Destroy();
}

// ============================================================================
// [DEBUG] OnHit (폭발/지형 충돌)
// ============================================================================
void AF1Projectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority()) return;

	// [DEBUG] OnHit 발생
	UE_LOG(LogTemp, Log, TEXT("[Projectile] OnHit! Hitted Actor: %s"), *GetNameSafe(OtherActor));

	FVector EffectLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f);
	FVector EffectScale = FVector(0.2f);

	MulticastSpawnImpactEffect(EffectLocation, EffectScale);

	TArray<FOverlapResult> Overlaps;
	FVector Origin = GetActorLocation();
	float ExplosionRadius = 200.0f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetOwner()) Params.AddIgnoredActor(GetOwner());

	// 디버그 구체 그리기 (범위 확인용)
	DrawDebugSphere(GetWorld(), Origin, ExplosionRadius, 12, FColor::Red, false, 2.0f);

	bool bHitFound = GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn), // Pawn만 검색
		FCollisionShape::MakeSphere(ExplosionRadius),
		Params
	);

	if (bHitFound)
	{
		for (const FOverlapResult& Result : Overlaps)
		{
			if (AActor* TargetActor = Result.GetActor())
			{
				AActor* ProjectileOwner = GetOwner();
				IF1CombatInterface* SourceCombat = Cast<IF1CombatInterface>(ProjectileOwner);

				// [수정] OtherActor(벽)가 아니라 TargetActor(적)를 검사해야 함!!
				IF1CombatInterface* TargetCombat = Cast<IF1CombatInterface>(TargetActor);

				if (SourceCombat && TargetCombat)
				{
					if (SourceCombat->GetTeamID() == TargetCombat->GetTeamID())
					{
						continue; // 팀킬 방지 (return 하면 다른 적도 못 때림 -> continue로 변경)
					}
				}

				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
				{
					if (DamageEffectSpecHandle.Data.IsValid())
					{
						TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
						UE_LOG(LogTemp, Warning, TEXT("[Projectile] Explosion Hit: %s"), *GetNameSafe(TargetActor));
					}
				}
			}
		}
	}

	SetLifeSpan(3.0f);
	if (Sphere) Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorHiddenInGame(true);
}

void AF1Projectile::MulticastSpawnImpactEffect_Implementation(FVector Location, FVector Scale)
{
	bHit = true;
	SetActorHiddenInGame(true);
	if (Sphere) Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, Location, FRotator::ZeroRotator, Scale);
	}
}