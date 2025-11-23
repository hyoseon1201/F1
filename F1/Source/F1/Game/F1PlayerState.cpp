// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/F1PlayerState.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "Net/UnrealNetwork.h"

AF1PlayerState::AF1PlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UF1AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UF1AttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
}

void AF1PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AF1PlayerState, TeamID);
}

void AF1PlayerState::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	// 서버에서만 설정 가능
	if (HasAuthority())
	{
		TeamID = NewTeamID;
	}
}

FGenericTeamId AF1PlayerState::GetGenericTeamId() const
{
	return TeamID;
}

void AF1PlayerState::OnRep_TeamID()
{
	// 팀 정보가 변경되면 필요한 로직 수행 (예: 캐릭터 색상 변경 등)
}

UAbilitySystemComponent* AF1PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
