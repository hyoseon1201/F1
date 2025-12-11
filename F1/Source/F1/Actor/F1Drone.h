

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "F1Drone.generated.h"

UCLASS()
class F1_API AF1Drone : public AActor
{
	GENERATED_BODY()
	
public:	
	AF1Drone();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	void FollowOwner(float DeltaTime);

	void ScanAndAttack();

	AActor* FindBestTarget(const TArray<AActor*>& OverlappingActors);

	void FireProjectile(AActor* Target);

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly)
	float ScanRadius = 800.f;

	UPROPERTY(EditDefaultsOnly)
	float AttackInterval = 1.0f;

	FTimerHandle TimerHandle_Attack;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AF1Projectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameplayEffect> DamageEffectClass;
};
