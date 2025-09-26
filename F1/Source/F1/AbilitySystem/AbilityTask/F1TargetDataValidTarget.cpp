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

	// Ÿ�� ��ȿ�� �˻�
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
	// ... ������ �ڵ�

	// ��ȿ�� Ÿ�� - ���� ������� ����
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

	// ���������� Ÿ�� ��ȿ�� ��˻�
	if (DataHandle.Num() > 0)
	{
		const FGameplayAbilityTargetData* TargetData = DataHandle.Get(0);
		if (TargetData)
		{
			const FHitResult* HitResult = TargetData->GetHitResult();
			if (HitResult && !IsValidTarget(HitResult->GetActor()))
			{
				// �������� ��ȿ���� �ʴٰ� ����
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

	// AbilityTask������ Ability�� ���� ����
	AActor* SourceActor = Ability->GetAvatarActorFromActorInfo();
	if (!SourceActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[TargetDataValidTarget] SourceActor is NULL"));
		return false;
	}

	// �� �������̽��� ���
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

	// �� �������̽��� ������ ���� �ƴ�
	UE_LOG(LogTemp, Error, TEXT("[TargetDataValidTarget] Team interface not implemented on Source or Target"));
	return false;
}

bool UF1TargetDataValidTarget::IsAlive(AActor* Target) const
{
	if (!Target)
	{
		return false;
	}

	// TODO: ü�� �ý����� ���� ���� ����
	// ����: �±׸� ���� ������ ���� ����
	if (Target->ActorHasTag(FName("Dead")))
	{
		return false;
	}

	// �Ǵ� ü�� ������Ʈ�� ���� ����
	// if (UHealthComponent* HealthComp = Target->FindComponentByClass<UHealthComponent>())
	// {
	//     return HealthComp->IsAlive();
	// }

	return true;
}
