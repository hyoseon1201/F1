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

void AF1Projectile::SetHomingTarget(AActor* TargetActor)
{
    if (TargetActor && ProjectileMovement)
    {
        // 1. 유도 기능 활성화
        ProjectileMovement->bIsHomingProjectile = true;

        // 2. 유도 가속도 설정 (이게 높아야 확 꺾어서 쫓아갑니다)
        // 블루프린트에서 설정한 값을 덮어쓸 수도 있으니, 필요하면 이 줄은 주석 처리하고 BP에서 조절하세요.
        ProjectileMovement->HomingAccelerationMagnitude = 20000.f;

        // 3. [핵심] 누구를 쫓아갈지 설정 (타겟의 뿌리 컴포넌트)
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
    // 1. 유효성 검사 (공통)
    AActor* ProjectileOwner = GetOwner();
    if (!ProjectileOwner) return;
    if (OtherActor == ProjectileOwner) return; // 내가 쏜 것에 내가 맞지 않게 (옵션)

    // 2. 이미 맞았는지 체크 (클라이언트 중복 재생 방지)
    if (bHit) return;

    // 3. 팀 체크 (공통 - 클라이언트도 팀 정보를 알아야 함)
    // 주의: PlayerState나 Character의 TeamID가 리플리케이션 되고 있어야 정상 동작합니다.
    IGenericTeamAgentInterface* SourceTeamAgent = Cast<IGenericTeamAgentInterface>(ProjectileOwner);
    IGenericTeamAgentInterface* TargetTeamAgent = Cast<IGenericTeamAgentInterface>(OtherActor);

    if (SourceTeamAgent && TargetTeamAgent)
    {
        if (SourceTeamAgent->GetGenericTeamId() == TargetTeamAgent->GetGenericTeamId())
        {
            return; // 같은 팀이면 그냥 통과하거나 무시
        }
    }

    // 4. [시각 효과] 권한과 상관없이 모두 수행 (서버 + 클라이언트)
    // 여기서 소리와 이펙트를 재생해야 클라이언트 눈에 보입니다.
    if (ImpactEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, GetActorLocation());
    }

    // (옵션) 사운드 재생
    // UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());


    // 5. [서버 로직] 데미지 및 파괴 처리
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
        // 클라이언트는 서버가 Destroy() 하기 전까지 잠시 살아있을 수 있으므로,
        // '나는 이미 터졌다'고 표시해두고 추가 충돌을 막습니다.
        bHit = true;
    }
}
