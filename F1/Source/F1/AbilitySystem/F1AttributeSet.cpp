// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/F1AttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UF1AttributeSet::UF1AttributeSet()
{
	// TEMP

	// 기본 생존 능력치 초기화
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitHealthRegeneration(1.0f);
	InitMana(25.f);
	InitMaxMana(50.f);
	InitManaRegeneration(1.0f);

	// 공격 능력치 초기화
	InitAttackDamage(60.f);
	InitAttackSpeed(0.625f);
	InitAbilityPower(0.f);
	InitCriticalStrikeChance(0.f);
	InitCriticalStrikeDamage(200.f);

	// 방어 능력치 초기화
	InitArmor(30.f);
	InitMagicResistance(30.f);

	// 이동 및 유틸리티 초기화
	InitMovementSpeed(325.f);
	InitAbilityHaste(0.f);

	// 관통력 초기화
	InitArmorPenetration(0.f);
	InitMagicPenetration(0.f);

	// 흡혈 초기화
	InitLifeSteal(0.f);
	InitOmnivamp(0.f);

	// 저항력 초기화
	InitTenacity(0.f);
	InitSlowResistance(0.f);

	// 사거리 초기화
	InitAttackRange(125.f);
}

void UF1AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 기본 생존 능력치
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);

	// 공격 능력치
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, AbilityPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, CriticalStrikeChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, CriticalStrikeDamage, COND_None, REPNOTIFY_Always);

	// 방어 능력치
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MagicResistance, COND_None, REPNOTIFY_Always);

	// 이동 및 유틸리티
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, AbilityHaste, COND_None, REPNOTIFY_Always);

	// 관통력
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MagicPenetration, COND_None, REPNOTIFY_Always);

	// 흡혈
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, LifeSteal, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Omnivamp, COND_None, REPNOTIFY_Always);

	// 저항력
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Tenacity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, SlowResistance, COND_None, REPNOTIFY_Always);

	// 사거리
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, AttackRange, COND_None, REPNOTIFY_Always);
}

void UF1AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UF1AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}
}

// ===========================================
// OnRep 함수들 구현
// ===========================================

void UF1AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, Health, OldHealth);
}

void UF1AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, MaxHealth, OldMaxHealth);
}

void UF1AttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UF1AttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, Mana, OldMana);
}

void UF1AttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, MaxMana, OldMaxMana);
}

void UF1AttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UF1AttributeSet::OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, AttackDamage, OldAttackDamage);
}

void UF1AttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, AttackSpeed, OldAttackSpeed);
}

void UF1AttributeSet::OnRep_AbilityPower(const FGameplayAttributeData& OldAbilityPower) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, AbilityPower, OldAbilityPower);
}

void UF1AttributeSet::OnRep_CriticalStrikeChance(const FGameplayAttributeData& OldCriticalStrikeChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, CriticalStrikeChance, OldCriticalStrikeChance);
}

void UF1AttributeSet::OnRep_CriticalStrikeDamage(const FGameplayAttributeData& OldCriticalStrikeDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, CriticalStrikeDamage, OldCriticalStrikeDamage);
}

void UF1AttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, Armor, OldArmor);
}

void UF1AttributeSet::OnRep_MagicResistance(const FGameplayAttributeData& OldMagicResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, MagicResistance, OldMagicResistance);
}

void UF1AttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, MovementSpeed, OldMovementSpeed);
}

void UF1AttributeSet::OnRep_AbilityHaste(const FGameplayAttributeData& OldAbilityHaste) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, AbilityHaste, OldAbilityHaste);
}

void UF1AttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UF1AttributeSet::OnRep_MagicPenetration(const FGameplayAttributeData& OldMagicPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, MagicPenetration, OldMagicPenetration);
}

void UF1AttributeSet::OnRep_LifeSteal(const FGameplayAttributeData& OldLifeSteal) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, LifeSteal, OldLifeSteal);
}

void UF1AttributeSet::OnRep_Omnivamp(const FGameplayAttributeData& OldOmnivamp) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, Omnivamp, OldOmnivamp);
}

void UF1AttributeSet::OnRep_Tenacity(const FGameplayAttributeData& OldTenacity) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, Tenacity, OldTenacity);
}

void UF1AttributeSet::OnRep_SlowResistance(const FGameplayAttributeData& OldSlowResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, SlowResistance, OldSlowResistance);
}

void UF1AttributeSet::OnRep_AttackRange(const FGameplayAttributeData& OldAttackRange) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, AttackRange, OldAttackRange);
}

void UF1AttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			ACharacter* SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}