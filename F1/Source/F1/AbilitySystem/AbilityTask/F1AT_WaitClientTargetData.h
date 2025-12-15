#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "F1AT_WaitClientTargetData.generated.h"

UENUM(BlueprintType)
enum class EF1TargetingType : uint8
{
	Ground		UMETA(DisplayName = "Ground (Non-Target)"), // 땅, 적 상관없이 위치만
	Enemy		UMETA(DisplayName = "Enemy Only (Targeting)") // 적만 (인터페이스 체크)
};

// 데이터가 도착했을 때 실행할 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitTargetDataDelegate, const FGameplayAbilityTargetDataHandle&, Data);

/**
 * 클라이언트가 계산한 데이터를 서버가 기다리는 태스크
 */
UCLASS()
class F1_API UF1AT_WaitClientTargetData : public UAbilityTask
{
	GENERATED_BODY()

public:
	// 1. 팩토리 함수 (BP와 C++에서 이 태스크를 만들기 위해 호출)
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UF1AT_WaitClientTargetData* WaitClientTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName, EF1TargetingType TargetingType, AActor* OptionalExternalTarget = nullptr);
	
	// 2. 태스크가 시작될 때 실행되는 함수
	virtual void Activate() override;

	// 3. 서버가 데이터를 받았을 때 호출되는 콜백
	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);

	// 4. 델리게이트 (블루프린트나 어빌리티에서 바인딩)
	UPROPERTY(BlueprintAssignable)
	FWaitTargetDataDelegate ValidData;

protected:
	// 타겟 데이터 전송 로직
	void SendTargetDataToServer();

	EF1TargetingType TType;

	UPROPERTY()
	TObjectPtr<AActor> ExternalTarget;
};