// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include <GenericTeamAgentInterface.h>

#include "F1PlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

/**
 * 
 */
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

protected:
    // 복제되어야 함 (모두가 내 팀을 알아야 하니까)
    UPROPERTY(ReplicatedUsing = OnRep_TeamID, VisibleAnywhere)
    FGenericTeamId TeamID = FGenericTeamId::NoTeam;

    UFUNCTION()
    void OnRep_TeamID();

    // ===========================================
    // GAS
    // ===========================================
public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    UAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<UAttributeSet> AttributeSet;
};
