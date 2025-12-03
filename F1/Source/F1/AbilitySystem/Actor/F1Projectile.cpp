// Fill out your copyright notice in the Description page of Project Settings.

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
#include "Engine/OverlapResult.h" // 필수

AF1Projectile::AF1Projectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);

	// 충돌 프로필 설정 (BP에서 Custom 설정을 따르도록 함)
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
	// 직사(Overlap)로 터졌을 때 클라이언트 이펙트 처리 (기존 유지)
	if (!bHit && !HasAuthority())
	{
		if (ImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, GetActorLocation());
		}
	}
	Super::Destroyed();
}

void AF1Projectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 직사 공격 로직 (기존 코드 유지)
	AActor* ProjectileOwner = GetOwner();
	if (!ProjectileOwner) return;
	if (OtherActor == ProjectileOwner) return;

	if (bHit) return;

	IGenericTeamAgentInterface* SourceTeamAgent = Cast<IGenericTeamAgentInterface>(ProjectileOwner);
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(OtherActor);

	if (SourceTeamAgent && TargetTeamAgent)
	{
		if (SourceTeamAgent->GetGenericTeamId() == TargetTeamAgent->GetGenericTeamId())
		{
			return;
		}
	}

	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, GetActorLocation());
	}

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			if (DamageEffectSpecHandle.Data.IsValid())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
			}
		}
		Destroy();
	}
	else
	{
		bHit = true;
	}
}

void AF1Projectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// [중요] Hit 이벤트는 서버에서만 처리하고 결과를 전파합니다.
	if (!HasAuthority()) return;

	// 1. 이펙트 위치 및 스케일 계산 (서버가 결정)
	FVector EffectLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f);
	FVector EffectScale = FVector(0.2f);

	// 2. 모든 클라이언트에게 "이 위치, 이 크기로 터뜨려라" 명령
	MulticastSpawnImpactEffect(EffectLocation, EffectScale);

	// 3. 광역 데미지 처리
	TArray<FOverlapResult> Overlaps;
	FVector Origin = GetActorLocation();
	float ExplosionRadius = 200.0f;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetOwner()) Params.AddIgnoredActor(GetOwner());

	bool bHitFound = GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		Origin,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
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
				IGenericTeamAgentInterface* SourceTeamAgent = Cast<IGenericTeamAgentInterface>(ProjectileOwner);
				IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(TargetActor);

				if (SourceTeamAgent && TargetTeamAgent)
				{
					if (SourceTeamAgent->GetGenericTeamId() == TargetTeamAgent->GetGenericTeamId())
					{
						continue; // 아군은 패스
					}
				}

				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
				{
					if (DamageEffectSpecHandle.Data.IsValid())
					{
						TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
					}
				}
			}
		}
	}

	// 4. 삭제 (네트워크 지연을 고려해 약간의 유예를 둠)
	SetLifeSpan(3.0f);

	if (Sphere) Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorHiddenInGame(true);
}

void AF1Projectile::MulticastSpawnImpactEffect_Implementation(FVector Location, FVector Scale)
{
	// [핵심 추가] 클라이언트에게 "나 이미 맞았어"라고 알려줌
	// 이걸 해줘야 나중에 Destroyed()가 호출될 때 중복 재생을 안 함.
	bHit = true;

	// [추가 팁] 클라이언트에서도 즉시 숨겨버려서 반응성을 높임
	// (서버에서 Hidden을 해도 리플리케이션 딜레이가 있을 수 있으므로)
	SetActorHiddenInGame(true);
	if (Sphere) Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// [모든 클라이언트 실행] 서버가 준 위치와 크기로 이펙트 재생
	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this,
			ImpactEffect,
			Location,
			FRotator::ZeroRotator,
			Scale
		);

		// (필요시 사운드 재생 코드 추가)
	}
}