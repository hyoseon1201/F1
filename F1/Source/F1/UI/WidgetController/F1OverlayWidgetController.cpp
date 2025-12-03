#include "UI/WidgetController/F1OverlayWidgetController.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "AbilitySystem/F1AbilitySystemComponent.h"

void UF1OverlayWidgetController::BroadcastInitialValues()
{
	// [핵심] 부모(Hero)의 방송을 먼저 실행합니다.
	// 여기서 체력, 마나, 레벨, 닉네임, 스킬 아이콘이 방송됩니다.
	Super::BroadcastInitialValues();

	// 이제 자식만의 추가 스탯을 방송합니다.
	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);

	OnHealthRegenerationChanged.Broadcast(F1AttributeSet->GetHealthRegeneration());
	OnManaRegenerationChanged.Broadcast(F1AttributeSet->GetManaRegeneration());

	OnAttackDamageChanged.Broadcast(F1AttributeSet->GetAttackDamage());
	OnAttackSpeedChanged.Broadcast(F1AttributeSet->GetAttackSpeed());
	OnAbilityPowerChanged.Broadcast(F1AttributeSet->GetAbilityPower());
	OnCriticalStrikeChanceChanged.Broadcast(F1AttributeSet->GetCriticalStrikeChance());
	OnCriticalStrikeDamageChanged.Broadcast(F1AttributeSet->GetCriticalStrikeDamage());

	OnArmorChanged.Broadcast(F1AttributeSet->GetArmor());
	OnMagicResistanceChanged.Broadcast(F1AttributeSet->GetMagicResistance());

	OnMovementSpeedChanged.Broadcast(F1AttributeSet->GetMovementSpeed());
	OnAbilityHasteChanged.Broadcast(F1AttributeSet->GetAbilityHaste());

	OnArmorPenetrationFlatChanged.Broadcast(F1AttributeSet->GetArmorPenetrationFlat());
	OnArmorPenetrationPercentChanged.Broadcast(F1AttributeSet->GetArmorPenetrationPercent());
	OnMagicPenetrationFlatChanged.Broadcast(F1AttributeSet->GetMagicPenetrationFlat());
	OnMagicPenetrationPercentChanged.Broadcast(F1AttributeSet->GetMagicPenetrationPercent());

	OnLifeStealChanged.Broadcast(F1AttributeSet->GetLifeSteal());
	OnOmnivampChanged.Broadcast(F1AttributeSet->GetOmnivamp());

	OnTenacityChanged.Broadcast(F1AttributeSet->GetTenacity());
	OnSlowResistanceChanged.Broadcast(F1AttributeSet->GetSlowResistance());

	OnAttackRangeChanged.Broadcast(F1AttributeSet->GetAttackRange());
}

void UF1OverlayWidgetController::BindCallbacksToDependencies()
{
	// [핵심] 부모(Hero)의 바인딩을 먼저 실행합니다.
	// 여기서 체력/마나 변경 감지, 스킬 쿨타임 감지가 연결됩니다.
	Super::BindCallbacksToDependencies();

	const UF1AttributeSet* F1AttributeSet = CastChecked<UF1AttributeSet>(AttributeSet);

	// [중복 삭제됨: Health, Mana 등은 부모가 함]

	// Health Regeneration
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetHealthRegenerationAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnHealthRegenerationChanged.Broadcast(Data.NewValue);
			});

	// Mana Regeneration
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetManaRegenerationAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnManaRegenerationChanged.Broadcast(Data.NewValue);
			});

	// Attack Damage
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAttackDamageAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				UE_LOG(LogTemp, Warning, TEXT("AttackDamage Changed"));
				OnAttackDamageChanged.Broadcast(Data.NewValue);
			});

	// Attack Speed
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAttackSpeedAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnAttackSpeedChanged.Broadcast(Data.NewValue);
			});

	// Ability Power
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAbilityPowerAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnAbilityPowerChanged.Broadcast(Data.NewValue);
			});

	// Critical Strike Chance
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetCriticalStrikeChanceAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnCriticalStrikeChanceChanged.Broadcast(Data.NewValue);
			});

	// Critical Strike Damage
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetCriticalStrikeDamageAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnCriticalStrikeDamageChanged.Broadcast(Data.NewValue);
			});

	// Armor
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetArmorAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnArmorChanged.Broadcast(Data.NewValue);
			});

	// Magic Resistance
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMagicResistanceAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMagicResistanceChanged.Broadcast(Data.NewValue);
			});

	// Movement Speed
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMovementSpeedAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMovementSpeedChanged.Broadcast(Data.NewValue);
			});

	// Ability Haste
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAbilityHasteAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnAbilityHasteChanged.Broadcast(Data.NewValue);
			});

	// Armor Penetration Flat
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetArmorPenetrationFlatAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnArmorPenetrationFlatChanged.Broadcast(Data.NewValue);
			});

	// Armor Penetration Percent
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetArmorPenetrationPercentAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnArmorPenetrationPercentChanged.Broadcast(Data.NewValue);
			});

	// Magic Penetration Flat
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMagicPenetrationFlatAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMagicPenetrationFlatChanged.Broadcast(Data.NewValue);
			});

	// Magic Penetration Percent
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetMagicPenetrationPercentAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMagicPenetrationPercentChanged.Broadcast(Data.NewValue);
			});

	// Life Steal
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetLifeStealAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnLifeStealChanged.Broadcast(Data.NewValue);
			});

	// Omnivamp
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetOmnivampAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnOmnivampChanged.Broadcast(Data.NewValue);
			});

	// Tenacity
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetTenacityAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnTenacityChanged.Broadcast(Data.NewValue);
			});

	// Slow Resistance
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetSlowResistanceAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnSlowResistanceChanged.Broadcast(Data.NewValue);
			});

	// Attack Range
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(F1AttributeSet->GetAttackRangeAttribute())
		.AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnAttackRangeChanged.Broadcast(Data.NewValue);
			});

	// EffectAssetTags 람다 (이건 유지해도 됨)
	Cast<UF1AbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, Msg);
			}
		}
	);
}