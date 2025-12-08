// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"

#include "F1PlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

/**
 * */
UCLASS()
class F1_API AF1PlayerState : public APlayerState,
	public IAbilitySystemInterface,
	public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	AF1PlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 팀 ID 설정/가져오기
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	// ===========================================
	// [추가] GAS
	// ===========================================
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	// ===========================================
	// [추가] 레벨 시스템
	// ===========================================
	// 레벨 가져오기
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }

	// 레벨 더하기 (예: AddToLevel(1))
	void AddToLevel(int32 InLevelToAdd);

	// 레벨 강제 설정 (예: SetPlayerLevel(18))
	void SetPlayerLevel(int32 InLevel);

protected:
	// 복제되어야 함 (모두가 내 팀을 알아야 하니까)
	UPROPERTY(ReplicatedUsing = OnRep_TeamID, VisibleAnywhere)
	FGenericTeamId TeamID = FGenericTeamId::NoTeam;

	UFUNCTION()
	void OnRep_TeamID();

	// ===========================================
	// [추가] 레벨 변수 (서버 -> 클라이언트 복제)
	// ===========================================
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level, Category = "F1|Stats")
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
};