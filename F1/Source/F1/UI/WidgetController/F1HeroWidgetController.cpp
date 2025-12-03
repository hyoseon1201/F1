// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/WidgetController/F1HeroWidgetController.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "AbilitySystem/F1AbilitySystemComponent.h" // [필수] ForEachAbility, GetActiveEffectsTimeRemaining
#include "Data/F1AbilityInfo.h"       // [필수] 데이터 에셋
#include "GameFramework/PlayerState.h"
#include "GameplayTagContainer.h"

void UF1HeroWidgetController::BroadcastInitialValues()
{
	// 1. 부모 클래스 로직 (안전빵)
	Super::BroadcastInitialValues();

	// 2. 필수 데이터 확인
	if (!GetF1ASC() || !AttributeSet) return;

	// ==============================================================
	// [Part 1] 속성(Attributes) 초기값 방송
	// ==============================================================
	const UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(F1AS->GetHealth());
	OnMaxHealthChanged.Broadcast(F1AS->GetMaxHealth());
	OnManaChanged.Broadcast(F1AS->GetMana());
	OnMaxManaChanged.Broadcast(F1AS->GetMaxMana());

	// 레벨 방송
	OnLevelChanged.Broadcast(static_cast<int32>(F1AS->GetCharacterLevel()));

	// ==============================================================
	// [Part 2] 플레이어 정보 방송
	// ==============================================================
	if (PlayerState)
	{
		OnNameChanged.Broadcast(PlayerState->GetPlayerName());
	}

	// ==============================================================
	// [Part 3] 스킬(Ability) 아이콘 정보 방송
	// ==============================================================
	if (AbilityInfo)
	{
		// ASC에 등록된 어빌리티들을 순회하며 UI에 필요한 정보 방송
		// (람다 인자 타입을 FGameplayAbilitySpec으로 맞춰서 템플릿 에러 방지)
		GetF1ASC()->ForEachAbility(
			[this](const FGameplayAbilitySpec& AbilitySpec) -> bool
			{
				// 3-1. InputTag 찾기 (InputTag.Q 등)
				FGameplayTag InputTag = FGameplayTag();
				for (const FGameplayTag& Tag : AbilitySpec.DynamicAbilityTags)
				{
					if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
					{
						InputTag = Tag;
						break;
					}
				}

				// 3-2. InputTag가 있으면 UI에 등록
				if (InputTag.IsValid())
				{
					FGameplayTag AbilityTag = FGameplayTag();
					if (AbilitySpec.Ability->AbilityTags.Num() > 0)
					{
						AbilityTag = AbilitySpec.Ability->AbilityTags.GetByIndex(0);
					}

					// 데이터 에셋에서 아이콘 등 정보 찾기 (로그 끄기 false)
					FF1AbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag, false);

					// 찾았다면 방송
					if (Info.AbilityTag.IsValid())
					{
						Info.InputTag = InputTag; // 현재 할당된 키 정보 덮어쓰기
						AbilityInfoDelegate.Broadcast(Info);
					}
				}

				return true;
			}
		);
	}
}

void UF1HeroWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	UE_LOG(LogTemp, Warning, TEXT("[UI] BindCallbacks Called! IsServer: %d"), GetWorld()->GetNetMode() < NM_Client);

	const UF1AttributeSet* F1AS = CastChecked<UF1AttributeSet>(AttributeSet);
	if (!GetF1ASC()) return;

	// ==============================================================
	// 1. 속성 변경 감지 (람다)
	// ==============================================================
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetHealthAttribute()).AddLambda(
		[this, F1AS](const FOnAttributeChangeData& Data) {
			OnHealthChanged.Broadcast(FMath::Clamp(Data.NewValue, 0.f, F1AS->GetMaxHealth()));
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxHealthChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetManaAttribute()).AddLambda(
		[this, F1AS](const FOnAttributeChangeData& Data) {
			OnManaChanged.Broadcast(FMath::Clamp(Data.NewValue, 0.f, F1AS->GetMaxMana()));
		}
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnMaxManaChanged.Broadcast(Data.NewValue); }
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AS->GetCharacterLevelAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data) { OnLevelChanged.Broadcast(static_cast<int32>(Data.NewValue)); }
	);

	// ==============================================================
	// 2. 쿨타임 태그 변경 감지 (Cooldown Tag)
	// ==============================================================
	if (AbilityInfo)
	{
		for (const FF1AbilityInfo& Info : AbilityInfo->AbilityInformation)
		{
			if (Info.CooldownTag.IsValid())
			{
				// 해당 쿨타임 태그가 추가되거나(쿨 시작) 제거될 때(쿨 끝) 알림 받기
				GetF1ASC()->RegisterGameplayTagEvent(
					Info.CooldownTag,
					EGameplayTagEventType::NewOrRemoved
				).AddUObject(this, &UF1HeroWidgetController::OnAbilityTagsChanged);
			}
		}
	}

	GetF1ASC()->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UF1HeroWidgetController::OnActiveGameplayEffectAdded);
}

void UF1HeroWidgetController::OnAbilityTagsChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	// 쿨타임 태그가 붙음 (NewCount > 0) -> 쿨타임 시작
	// 쿨타임 태그가 떨어짐 (NewCount == 0) -> 쿨타임 종료

	if (NewCount > 0)
	{
		// 해당 태그를 가진 활성 이펙트(GE_Cooldown)의 남은 시간을 찾음
		FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CallbackTag));

		// ASC에 추가해둔 헬퍼 함수 사용 (가장 긴 시간 반환)
		TArray<float> Times = GetF1ASC()->GetActiveEffectsTimeRemaining(Query);

		if (Times.Num() > 0)
		{
			float TimeRemaining = Times[0];
			OnCooldownChanged.Broadcast(CallbackTag, TimeRemaining);
		}
	}
	else
	{
		// 쿨타임 끝남 -> 0초 전송 (UI 끄기용)
		OnCooldownChanged.Broadcast(CallbackTag, 0.f);
	}
}

void UF1HeroWidgetController::OnActiveGameplayEffectAdded(UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{

	UE_LOG(LogTemp, Warning, TEXT("[UI] Effect Added Detected!"));

	// 1. 이 이펙트의 태그들을 가져옴
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllGrantedTags(AssetTags);

	for (auto Tag : AssetTags)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UI] - Effect Tag: %s"), *Tag.ToString());
	}

	// 2. "Cooldown" 태그가 있는지 검사 (최적화)
	FGameplayTagContainer CooldownTags;

	// 데이터 에셋에서 내 스킬들의 쿨타임 태그와 매칭되는지 확인
	FGameplayTag CooldownTag = FGameplayTag();
	if (AbilityInfo)
	{
		for (const FF1AbilityInfo& Info : AbilityInfo->AbilityInformation)
		{
			// 적용된 이펙트가 쿨타임 태그를 가지고 있다면
			if (AssetTags.HasTagExact(Info.CooldownTag))
			{
				CooldownTag = Info.CooldownTag;
				UE_LOG(LogTemp, Warning, TEXT("[UI] !!! Cooldown Tag MATCHED: %s !!!"), *CooldownTag.ToString());
				break;
			}
		}
	}

	// 3. 찾았으면 시간 방송
	if (CooldownTag.IsValid())
	{
		// 쿼리 생성
		FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(CooldownTag));

		// 남은 시간 배열 가져오기
		TArray<float> Times = GetF1ASC()->GetActiveEffectsTimeRemaining(Query);

		// [로그 1] 쿼리 결과 개수 확인
		// (여기서 0이 나오면 GE가 적용은 됐는데 'Active' 목록에 없다는 뜻)
		UE_LOG(LogTemp, Warning, TEXT("[UI] Query Result Count: %d for Tag: %s"), Times.Num(), *CooldownTag.ToString());

		if (Times.Num() > 0)
		{
			float TimeRemaining = Times[0];

			// [로그 2] 실제 방송되는 시간 값 확인
			// (여기서 0.0이 나오면 '데이터는 있는데 시간 동기화가 아직 안 됨'을 의미)
			UE_LOG(LogTemp, Error, TEXT("[UI] Broadcasting Time: %f FROM [%s]"), TimeRemaining, *GetName());

			OnCooldownChanged.Broadcast(CooldownTag, TimeRemaining);
		}
		else
		{
			// [로그 3] 태그 매칭은 됐는데 시간을 못 가져옴
			UE_LOG(LogTemp, Error, TEXT("[UI] FOUND Tag but Time Array is EMPTY! (Effect might be expired or invalid)"));
		}
	}
}
