
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "F1MeteorCircle.generated.h"

UCLASS()
class F1_API AF1MeteorCircle : public AActor
{
	GENERATED_BODY()
	
public:	
	AF1MeteorCircle();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void ApplyPeriodicDamage();

public:
	// 범위 (Sphere)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* MeteorCircleMesh;

	// 데미지 스펙 (GA에서 넘겨받음)
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DamageRadius = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DamagePeriod = 0.5f; // 0.5초마다 타격

	FTimerHandle TimerHandle_Damage;
};
