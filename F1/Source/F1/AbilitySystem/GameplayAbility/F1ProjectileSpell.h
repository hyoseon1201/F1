#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/F1DamageGameplayAbility.h"
#include "AbilitySystem/AbilityTask/F1AT_WaitClientTargetData.h"
#include "F1ProjectileSpell.generated.h"

class AF1Projectile;
class UGameplayAbilityTargetData_LocationInfo;

/**
 * [GAS 정석 구조]
 * 1. Activate -> Task 시작 (TargetData 전송)
 * 2. OnClientDataReceived -> 데이터 받아서 변수에 저장 (Caching)
 * 3. AnimNotify -> SpawnProjectileExecution 호출 -> 저장된 위치로 발사
 */
UCLASS()
class F1_API UF1ProjectileSpell : public UF1DamageGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	// 태스크 생성 및 시작 헬퍼 함수
	void StartTargetDataTask();

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnHomingProjectile(AActor* HomingTarget);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnArcProjectile(const FVector& TargetLocation, float OverrideGravityZ = 0.f);

	UFUNCTION()
	void OnClientDataReceived(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectileExecution();

	UFUNCTION(BlueprintCallable)
	void CacheTargetData(const FGameplayAbilityTargetDataHandle& DataHandle);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AF1Projectile> ProjectileClass;

	// --- 임시 저장용 변수들 (캐싱) ---

	// 어떤 모드로 쏠지 (주의: 클라/서버 싱크를 위해 보통은 별도 어빌리티로 분리하지만, 여기선 변수로 처리)
	enum class EProjectileSpawnMode { None, Linear, Homing, Arc };
	EProjectileSpawnMode SpawnMode = EProjectileSpawnMode::None;

	// 서버가 기억하고 있을 타겟 위치
	FVector CachedTargetLocation;

	// 서버가 기억하고 있을 유도 타겟
	UPROPERTY()
	AActor* CachedHomingTarget;

	// 곡사 중력값
	float CachedGravityZ;

	// 공통 스폰 마무리 헬퍼
	void FinishSpawningProjectile(AF1Projectile* Projectile, const FVector& HitLocation);

	void StartTargetDataTask(EF1TargetingType TargetingType);
};