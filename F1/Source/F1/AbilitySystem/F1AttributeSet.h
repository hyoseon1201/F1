// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "F1AttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties() {}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

/**
 *
 */
UCLASS()
class F1_API UF1AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// ===========================================
	// 기본 생존 능력치 (Vital Stats)
	// ===========================================
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, MaxHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Vital Attributes")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, HealthRegeneration);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, Mana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, MaxMana);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegeneration, Category = "Vital Attributes")
	FGameplayAttributeData ManaRegeneration;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, ManaRegeneration);

	// ===========================================
	// 공격 능력치 (Offensive Stats)
	// ===========================================
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackDamage, Category = "Offensive Attributes")
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, AttackDamage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackSpeed, Category = "Offensive Attributes")
	FGameplayAttributeData AttackSpeed;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, AttackSpeed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AbilityPower, Category = "Offensive Attributes")
	FGameplayAttributeData AbilityPower;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, AbilityPower);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalStrikeChance, Category = "Offensive Attributes")
	FGameplayAttributeData CriticalStrikeChance;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, CriticalStrikeChance);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalStrikeDamage, Category = "Offensive Attributes")
	FGameplayAttributeData CriticalStrikeDamage;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, CriticalStrikeDamage);

	// ===========================================
	// 방어 능력치 (Defensive Stats)
	// ===========================================
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Defensive Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, Armor);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicResistance, Category = "Defensive Attributes")
	FGameplayAttributeData MagicResistance;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, MagicResistance);

	// ===========================================
	// 이동 및 유틸리티 (Movement & Utility)
	// ===========================================
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MovementSpeed, Category = "Movement Attributes")
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, MovementSpeed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AbilityHaste, Category = "Utility Attributes")
	FGameplayAttributeData AbilityHaste;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, AbilityHaste);

	// ===========================================
	// 관통력 (Penetration)
	// ===========================================
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetrationFlat, Category = "Penetration Attributes")
	FGameplayAttributeData ArmorPenetrationFlat;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, ArmorPenetrationFlat);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetrationPercent, Category = "Penetration Attributes")
	FGameplayAttributeData ArmorPenetrationPercent;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, ArmorPenetrationPercent);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicPenetrationFlat, Category = "Penetration Attributes")
	FGameplayAttributeData MagicPenetrationFlat;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, MagicPenetrationFlat);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicPenetrationPercent, Category = "Penetration Attributes")
	FGameplayAttributeData MagicPenetrationPercent;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, MagicPenetrationPercent);

	// ===========================================
	// 흡혈 (Life Steal & Spell Vamp)
	// ===========================================
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LifeSteal, Category = "Sustain Attributes")
	FGameplayAttributeData LifeSteal;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, LifeSteal);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Omnivamp, Category = "Sustain Attributes")
	FGameplayAttributeData Omnivamp;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, Omnivamp);

	// ===========================================
	// 저항력 (Resistances)
	// ===========================================
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Tenacity, Category = "Resistance Attributes")
	FGameplayAttributeData Tenacity;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, Tenacity);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SlowResistance, Category = "Resistance Attributes")
	FGameplayAttributeData SlowResistance;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, SlowResistance);

	// ===========================================
	// 사거리 (Range)
	// ===========================================
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackRange, Category = "Range Attributes")
	FGameplayAttributeData AttackRange;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, AttackRange);

	// ===========================================
	// 메타 정보 (Meta)
	// ===========================================
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, IncomingDamage);

	// ===========================================
	// 캐릭터 정보 (Character Info)
	// ===========================================
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CharacterLevel, Category = "Character Info")
	FGameplayAttributeData CharacterLevel;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, CharacterLevel);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Experience, Category = "Character Info")
	FGameplayAttributeData Experience;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, Experience);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxExperience, Category = "Character Info")
	FGameplayAttributeData MaxExperience;
	ATTRIBUTE_ACCESSORS(UF1AttributeSet, MaxExperience);

public:

	UF1AttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// ===========================================
	// OnRep_Func
	// ===========================================
public:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;

	UFUNCTION()
	void OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage) const;

	UFUNCTION()
	void OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed) const;

	UFUNCTION()
	void OnRep_AbilityPower(const FGameplayAttributeData& OldAbilityPower) const;

	UFUNCTION()
	void OnRep_CriticalStrikeChance(const FGameplayAttributeData& OldCriticalStrikeChance) const;

	UFUNCTION()
	void OnRep_CriticalStrikeDamage(const FGameplayAttributeData& OldCriticalStrikeDamage) const;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;

	UFUNCTION()
	void OnRep_MagicResistance(const FGameplayAttributeData& OldMagicResistance) const;

	UFUNCTION()
	void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed) const;

	UFUNCTION()
	void OnRep_AbilityHaste(const FGameplayAttributeData& OldAbilityHaste) const;

	UFUNCTION()
	void OnRep_ArmorPenetrationFlat(const FGameplayAttributeData& OldArmorPenetrationFlat) const;

	UFUNCTION()
	void OnRep_ArmorPenetrationPercent(const FGameplayAttributeData& OldArmorPenetrationPercent) const;

	UFUNCTION()
	void OnRep_MagicPenetrationFlat(const FGameplayAttributeData& OldMagicPenetrationFlat) const;

	UFUNCTION()
	void OnRep_MagicPenetrationPercent(const FGameplayAttributeData& OldMagicPenetrationPercent) const;

	UFUNCTION()
	void OnRep_LifeSteal(const FGameplayAttributeData& OldLifeSteal) const;

	UFUNCTION()
	void OnRep_Omnivamp(const FGameplayAttributeData& OldOmnivamp) const;

	UFUNCTION()
	void OnRep_Tenacity(const FGameplayAttributeData& OldTenacity) const;

	UFUNCTION()
	void OnRep_SlowResistance(const FGameplayAttributeData& OldSlowResistance) const;

	UFUNCTION()
	void OnRep_AttackRange(const FGameplayAttributeData& OldAttackRange) const;

	UFUNCTION()
	void OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel) const;

	UFUNCTION()
	void OnRep_Experience(const FGameplayAttributeData& OldExperience) const;

	UFUNCTION()
	void OnRep_MaxExperience(const FGameplayAttributeData& OldMaxExperience) const;

private:
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
};
