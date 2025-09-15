// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/F1AttributeSet.h"
#include "Net/UnrealNetwork.h"

UF1AttributeSet::UF1AttributeSet()
{
	// �⺻ ���� �ɷ�ġ �ʱ�ȭ
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitHealthRegeneration(1.0f);
	InitMana(25.f);
	InitMaxMana(50.f);
	InitManaRegeneration(1.0f);

	// ���� �ɷ�ġ �ʱ�ȭ
	InitAttackDamage(60.f);
	InitAttackSpeed(0.625f);  // LOL �⺻ ����
	InitAbilityPower(0.f);
	InitCriticalStrikeChance(0.f);
	InitCriticalStrikeDamage(200.f);  // ġ��Ÿ �⺻ 200%

	// ��� �ɷ�ġ �ʱ�ȭ
	InitArmor(30.f);
	InitMagicResistance(30.f);

	// �̵� �� ��ƿ��Ƽ �ʱ�ȭ
	InitMovementSpeed(325.f);  // LOL �⺻ �̼�
	InitAbilityHaste(0.f);

	// ����� �ʱ�ȭ
	InitArmorPenetration(0.f);
	InitMagicPenetration(0.f);

	// ���� �ʱ�ȭ
	InitLifeSteal(0.f);
	InitOmnivamp(0.f);

	// ���׷� �ʱ�ȭ
	InitTenacity(0.f);
	InitSlowResistance(0.f);

	// ��Ÿ� �ʱ�ȭ
	InitAttackRange(125.f);  // ���� ĳ���� �⺻ ��Ÿ�
}

void UF1AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// �⺻ ���� �ɷ�ġ
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);

	// ���� �ɷ�ġ
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, AbilityPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, CriticalStrikeChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, CriticalStrikeDamage, COND_None, REPNOTIFY_Always);

	// ��� �ɷ�ġ
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MagicResistance, COND_None, REPNOTIFY_Always);

	// �̵� �� ��ƿ��Ƽ
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, AbilityHaste, COND_None, REPNOTIFY_Always);

	// �����
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MagicPenetration, COND_None, REPNOTIFY_Always);

	// ����
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, LifeSteal, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Omnivamp, COND_None, REPNOTIFY_Always);

	// ���׷�
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Tenacity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, SlowResistance, COND_None, REPNOTIFY_Always);

	// ��Ÿ�
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, AttackRange, COND_None, REPNOTIFY_Always);
}

// ===========================================
// OnRep �Լ��� ����
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