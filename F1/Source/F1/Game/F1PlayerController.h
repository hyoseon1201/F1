#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "F1PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UF1InputConfig;
class UF1AbilitySystemComponent;
class USplineComponent;
class UDamageTextComponent; // 전방 선언 확인
class AF1CharacterBase;

UCLASS()
class F1_API AF1PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AF1PlayerController();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// [RPC] 서버에 타겟 설정 요청
	UFUNCTION(Server, Reliable)
	void Server_SetTargetEnemy(AActor* NewTarget);

	// [RPC] 클라 이동 경로 동기화
	UFUNCTION(Client, Reliable)
	void Client_MoveToPoints(const TArray<FVector>& PathPoints);

	// [RPC] 클라 회전 동기화 (공격 시)
	UFUNCTION(Client, Reliable)
	void Client_FaceTarget(AActor* TargetToFace);

	// [RPC] 데미지 폰트 출력 (복구됨!)
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bCriticalHit);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> F1Context;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UF1InputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UF1AbilitySystemComponent> F1AbilitySystemComponent;

	UF1AbilitySystemComponent* GetASC();

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	void AutoRun();
	void TraceAndAttackTarget();
	void StartMovementToDestination();

	void CursorTrace();
	bool IsEnemy(AActor* Target);
	AActor* GetTargetUnderCursorWithAssist();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(Replicated)
	FVector CachedDestination = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bAutoRunning = false;

	UPROPERTY(Replicated)
	TObjectPtr<AActor> TargetEnemy = nullptr;

	// [회전용] 부드러운 회전을 위한 타겟 참조
	TWeakObjectPtr<AActor> FaceTargetActor;

	float FollowTime = 0.f;
	float ShortPressThreshold = 0.2f;

	// 공격 명령 스팸 방지 타이머
	float LastAttackTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float TargetingAssistRadius = 90.0f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> ThisActor;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> LastActor;

	FHitResult CursorHit;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};