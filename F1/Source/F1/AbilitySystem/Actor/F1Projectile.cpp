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

AF1Projectile::AF1Projectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetGenerateOverlapEvents(true);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AF1Projectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);

	if (Sphere)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AF1Projectile::OnSphereOverlap);
	}
}

void AF1Projectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, GetActorLocation());
	}
	Super::Destroyed();
}

void AF1Projectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;

	AActor* ProjectileOwner = GetOwner();
	if (!ProjectileOwner) return;

	// 팀 체크
	IGenericTeamAgentInterface* SourceTeamAgent = Cast<IGenericTeamAgentInterface>(ProjectileOwner);
	IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(OtherActor);

	if (SourceTeamAgent && TargetTeamAgent)
	{
		if (SourceTeamAgent->GetGenericTeamId() == TargetTeamAgent->GetGenericTeamId())
		{
			return; // 같은 팀
		}
	}

	// Impact Effect
	if (ImpactEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, GetActorLocation());
	}

	// 데미지 적용
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		if (DamageEffectSpecHandle.Data.IsValid())
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	}

	Destroy();
}
