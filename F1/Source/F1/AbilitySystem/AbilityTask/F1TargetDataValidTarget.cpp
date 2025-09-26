// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AbilityTask/F1TargetDataValidTarget.h"
#include "AbilitySystemComponent.h"
#include "Interaction/F1CombatInterface.h"
#include "GenericTeamAgentInterface.h"

UF1TargetDataValidTarget* UF1TargetDataValidTarget::CreateTargetDataValidTarget(UGameplayAbility* OwningAbility)
{
	UF1TargetDataValidTarget* MyObj = NewAbilityTask<UF1TargetDataValidTarget>(OwningAbility);
	return MyObj;
}

void UF1TargetDataValidTarget::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UF1TargetDataValidTarget::OnTargetDataReplicatedCallback);
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UF1TargetDataValidTarget::SendMouseCursorData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	AActor* TargetActor = CursorHit.GetActor();
	UE_LOG(LogTemp, Warning, TEXT("[TargetDataValidTarget] SendMouseCursorData - Hit Actor: %s"),
		TargetActor ? *TargetActor->GetName() : TEXT("NULL"));

	UE_LOG(LogTemp, Warning, TEXT("[TargetDataValidTarget] Hit Location: %s"), *CursorHit.Location.ToString());

	// 타겟 유효성 검사
	if (!IsValidTarget(TargetActor))
	{
		UE_LOG(LogTemp, Error, TEXT("[TargetDataValidTarget] Invalid target - Broadcasting NoValidTarget"));
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			NoValidTarget.Broadcast();
		}
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[TargetDataValidTarget] Valid target found - Broadcasting ValidTarget"));
	// ... 나머지 코드

	// 유효한 타겟 - 기존 로직대로 진행
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidTarget.Broadcast(DataHandle);
	}
}

void UF1TargetDataValidTarget::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	// 서버에서도 타겟 유효성 재검사
	if (DataHandle.Num() > 0)
	{
		const FGameplayAbilityTargetData* TargetData = DataHandle.Get(0);
		if (TargetData)
		{
			const FHitResult* HitResult = TargetData->GetHitResult();
			if (HitResult && !IsValidTarget(HitResult->GetActor()))
			{
				// 서버에서 유효하지 않다고 판정
				if (ShouldBroadcastAbilityTaskDelegates())
				{
					NoValidTarget.Broadcast();
				}
				return;
			}
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidTarget.Broadcast(DataHandle);
	}
}

bool UF1TargetDataValidTarget::IsValidTarget(AActor* Target) const
{
	UE_LOG(LogTemp, Warning, TEXT("[TargetDataValidTarget] IsValidTarget - Target: %s"),
		Target ? *Target->GetName() : TEXT("NULL"));

	if (!Target && bRequireActorTarget)
	{
		UE_LOG(LogTemp, Error, TEXT("[TargetDataValidTarget] Target is NULL but actor required"));
		return false;
	}

	if (bRequireEnemyTarget && !IsEnemy(Target))
	{
		UE_LOG(LogTemp, Error, TEXT("[TargetDataValidTarget] Target is not enemy"));
		return false;
	}

	if (bRequireAliveTarget && !IsAlive(Target))
	{
		UE_LOG(LogTemp, Error, TEXT("[TargetDataValidTarget] Target is not alive"));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("[TargetDataValidTarget] Target is valid"));
	return true;
}

bool UF1TargetDataValidTarget::IsEnemy(AActor* Target) const
{
	if (!Target)
	{
		return false;
	}

	// AbilityTask에서는 Ability를 통해 접근
	AActor* SourceActor = Ability->GetAvatarActorFromActorInfo();
	if (!SourceActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[TargetDataValidTarget] SourceActor is NULL"));
		return false;
	}

	// 팀 인터페이스만 사용
	IGenericTeamAgentInterface* SourceTeam = Cast<IGenericTeamAgentInterface>(SourceActor);
	IGenericTeamAgentInterface* TargetTeam = Cast<IGenericTeamAgentInterface>(Target);

	if (SourceTeam && TargetTeam)
	{
		FGenericTeamId SourceTeamId = SourceTeam->GetGenericTeamId();
		FGenericTeamId TargetTeamId = TargetTeam->GetGenericTeamId();

		UE_LOG(LogTemp, Warning, TEXT("[TargetDataValidTarget] SourceTeamId: %d, TargetTeamId: %d"),
			SourceTeamId.GetId(), TargetTeamId.GetId());

		return (SourceTeamId != TargetTeamId);
	}

	// 팀 인터페이스가 없으면 적이 아님
	UE_LOG(LogTemp, Error, TEXT("[TargetDataValidTarget] Team interface not implemented on Source or Target"));
	return false;
}

bool UF1TargetDataValidTarget::IsAlive(AActor* Target) const
{
	if (!Target)
	{
		return false;
	}

	// TODO: 체력 시스템을 통한 생존 판정
	// 예시: 태그를 통한 간단한 생존 판정
	if (Target->ActorHasTag(FName("Dead")))
	{
		return false;
	}

	// 또는 체력 컴포넌트를 통한 판정
	// if (UHealthComponent* HealthComp = Target->FindComponentByClass<UHealthComponent>())
	// {
	//     return HealthComp->IsAlive();
	// }

	return true;
}
