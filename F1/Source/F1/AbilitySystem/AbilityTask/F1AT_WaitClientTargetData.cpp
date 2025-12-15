#include "AbilitySystem/AbilityTask/F1AT_WaitClientTargetData.h"
#include "AbilitySystemComponent.h"
#include "Interaction/F1CombatInterface.h"

UF1AT_WaitClientTargetData* UF1AT_WaitClientTargetData::WaitClientTargetData(UGameplayAbility* OwningAbility, FName TaskInstanceName, EF1TargetingType TargetingType, AActor* OptionalExternalTarget)
{
	UF1AT_WaitClientTargetData* MyObj = NewAbilityTask<UF1AT_WaitClientTargetData>(OwningAbility, TaskInstanceName);
	MyObj->TType = TargetingType;
	MyObj->ExternalTarget = OptionalExternalTarget; // 저장!
	return MyObj;
}

void UF1AT_WaitClientTargetData::Activate()
{
	if (!Ability || !AbilitySystemComponent.Get()) return;

	// 1. [클라이언트] 데이터를 만들어서 서버로 전송
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Task] 1. Client: Activate() Called. Sending Data..."));
		SendTargetDataToServer();
	}
	// 2. [서버] 클라이언트가 보낸 데이터를 기다림 (이미 도착했을 수도 있음)
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Task] 1. Server: Activate() Called. Waiting for Data..."));

		// 델리게이트에 콜백 등록
		FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

		// 이미 데이터가 도착했는지 확인 후 델리게이트 바인딩
		AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UF1AT_WaitClientTargetData::OnTargetDataReplicatedCallback);

		// 혹시 바인딩 전에 이미 도착했다면? -> 즉시 호출
		const bool bCalledDelegate = AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

		if (!bCalledDelegate)
		{
			// 아직 안 왔다면 대기 상태로 유지
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UF1AT_WaitClientTargetData::SendTargetDataToServer()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	// -------------------------------------------------------------------------
	// [A] 외부 타겟이 있는 경우 (Auto Attack) -> 마우스 무시하고 이거 씀
	// -------------------------------------------------------------------------
	if (ExternalTarget)
	{
		bool bValid = false;

		// 유효성 검사 (적이 맞나?)
		if (TType == EF1TargetingType::Enemy)
		{
			if (ExternalTarget->Implements<UF1CombatInterface>())
			{
				bValid = true;
			}
		}
		else
		{
			bValid = true; // Ground 모드면 타겟만 있으면 OK
		}

		if (bValid)
		{
			// 데이터 포장 및 전송
			FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
			Data->HitResult.Location = ExternalTarget->GetActorLocation();
			Data->HitResult.HitObjectHandle = FActorInstanceHandle(ExternalTarget);
			Data->HitResult.bBlockingHit = true;

			FGameplayAbilityTargetDataHandle DataHandle;
			DataHandle.Add(Data);

			AbilitySystemComponent->ServerSetReplicatedTargetData(
				GetAbilitySpecHandle(),
				GetActivationPredictionKey(),
				DataHandle,
				FGameplayTag(),
				AbilitySystemComponent->ScopedPredictionKey
			);

			if (ShouldBroadcastAbilityTaskDelegates())
			{
				ValidData.Broadcast(DataHandle);
			}

			// 전송했으니 종료
			EndTask();
			return;
		}
	}

	// -------------------------------------------------------------------------
	// [B] 외부 타겟이 없는 경우 (Skill Shot) -> 기존 마우스 로직 실행
	// -------------------------------------------------------------------------

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if (!PC) { EndTask(); return; }

	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// -------------------------------------------------------------------------
	// [핵심 로직] 타입에 따른 검증 분기
	// -------------------------------------------------------------------------
	bool bValid = false;

	if (TType == EF1TargetingType::Ground)
	{
		// 1. Ground 모드: 어디든 부딪히기만 하면 OK (허공만 아니면 됨)
		if (CursorHit.bBlockingHit)
		{
			bValid = true;
		}
	}
	else if (TType == EF1TargetingType::Enemy)
	{
		// 2. Enemy 모드: 반드시 Actor가 있고 + 적군 인터페이스가 있어야 함
		AActor* HitActor = CursorHit.GetActor();
		if (HitActor && HitActor->Implements<UF1CombatInterface>())
		{
			bValid = true;
		}
	}

	// 검증 실패 시 (땅을 찍어야 하는데 하늘 찍음 / 적 찍어야 하는데 땅 찍음)
	if (!bValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Task] Invalid Target for Type [%d]. Canceled."), (int32)TType);
		EndTask(); // 그냥 조용히 종료 (몽타주 재생 안 됨)
		return;
	}

	// -------------------------------------------------------------------------
	// 성공! 데이터 전송
	// -------------------------------------------------------------------------
	UE_LOG(LogTemp, Warning, TEXT("[Task] Valid Target Found!"));

	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;

	FGameplayAbilityTargetDataHandle DataHandle;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey
	);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}

	EndTask();
}

void UF1AT_WaitClientTargetData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag)
{
	UE_LOG(LogTemp, Warning, TEXT("[Task] 3. Server: DATA RECEIVED!"));

	// 서버에서 실행됨: 데이터를 받았으니 델리게이트 송출
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(Data);
	}

	EndTask();
}