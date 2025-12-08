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
	DOREPLIFETIME(AF1PlayerState, Level);
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

void AF1PlayerState::AddToLevel(int32 InLevelToAdd)
{
	SetPlayerLevel(Level + InLevelToAdd);
}

void AF1PlayerState::SetPlayerLevel(int32 InLevel)
{
	// 서버에서만 변경 가능
	if (HasAuthority())
	{
		Level = InLevel;

		// 1. GAS AttributeSet에도 동기화 (데미지 계산용)
		if (AttributeSet)
		{
			// Cast가 필요할 수 있음 (UF1AttributeSet*)
			if (UF1AttributeSet* F1AS = Cast<UF1AttributeSet>(AttributeSet))
			{
				F1AS->SetCharacterLevel(Level);
			}
		}

		// 2. 서버라서 OnRep이 안 불리므로, 수동으로 UI 갱신 로직 실행 가능
		// (필요하다면 여기서 델리게이트 Broadcast)
	}
}

void AF1PlayerState::OnRep_Level(int32 OldLevel)
{
	// 현재 UI 업데이트는 AttributeSet의 변경을 감지해서 처리하므로,
	// 여기서는 특별한 로직이 없더라도 함수 몸통은 반드시 있어야 합니다.

	// (옵션) 로그 찍어보기
	// UE_LOG(LogTemp, Warning, TEXT("OnRep_Level: %d"), Level);
}

void AF1PlayerState::OnRep_TeamID()
{
	// 팀 정보가 변경되면 필요한 로직 수행 (예: 캐릭터 색상 변경 등)
}

UAbilitySystemComponent* AF1PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
