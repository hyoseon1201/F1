#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FF1GameplayTags
{
public:
    static const FF1GameplayTags& Get() { return GameplayTags; }
    static void InitializeNativeGameplayTags();

    // ===========================================
    // Attribute Tags - Vital Stats
    // ===========================================
    FGameplayTag Attributes_Vital_Health;
    FGameplayTag Attributes_Vital_MaxHealth;
    FGameplayTag Attributes_Vital_HealthRegeneration;
    FGameplayTag Attributes_Vital_Mana;
    FGameplayTag Attributes_Vital_MaxMana;
    FGameplayTag Attributes_Vital_ManaRegeneration;

    // ===========================================
    // Attribute Tags - Offensive Stats
    // ===========================================
    FGameplayTag Attributes_Offensive_AttackDamage;
    FGameplayTag Attributes_Offensive_AttackSpeed;
    FGameplayTag Attributes_Offensive_AbilityPower;
    FGameplayTag Attributes_Offensive_CriticalStrikeChance;
    FGameplayTag Attributes_Offensive_CriticalStrikeDamage;

    // ===========================================
    // Attribute Tags - Defensive Stats
    // ===========================================
    FGameplayTag Attributes_Defensive_Armor;
    FGameplayTag Attributes_Defensive_MagicResistance;

    // ===========================================
    // Attribute Tags - Movement & Utility
    // ===========================================
    FGameplayTag Attributes_Movement_Speed;
    FGameplayTag Attributes_Utility_AbilityHaste;

    // ===========================================
    // Attribute Tags - Penetration
    // ===========================================
    FGameplayTag Attributes_Penetration_Armor;
    FGameplayTag Attributes_Penetration_Magic;

    // ===========================================
    // Attribute Tags - Sustain
    // ===========================================
    FGameplayTag Attributes_Sustain_LifeSteal;
    FGameplayTag Attributes_Sustain_Omnivamp;

    // ===========================================
    // Attribute Tags - Resistance
    // ===========================================
    FGameplayTag Attributes_Resistance_Tenacity;
    FGameplayTag Attributes_Resistance_SlowResistance;

    // ===========================================
    // Attribute Tags - Range
    // ===========================================
    FGameplayTag Attributes_Range_Attack;

    // ===========================================
    // State Tags
    // ===========================================
    FGameplayTag State_Dead;
    FGameplayTag State_Stunned;
    FGameplayTag State_Silenced;
    FGameplayTag State_Rooted;
    FGameplayTag State_Invulnerable;

    // ===========================================
    // Ability Tags
    // ===========================================
    FGameplayTag Ability_Q;
    FGameplayTag Ability_W;
    FGameplayTag Ability_E;
    FGameplayTag Ability_R;
    FGameplayTag Ability_D;
    FGameplayTag Ability_F;
    FGameplayTag Ability_BasicAttack;

    // ===========================================
    // Input Tags
    // ===========================================
    FGameplayTag InputTag_Q;
    FGameplayTag InputTag_W;
    FGameplayTag InputTag_E;
    FGameplayTag InputTag_R;
    FGameplayTag InputTag_D;
    FGameplayTag InputTag_F;
    FGameplayTag InputTag_BasicAttack;
    FGameplayTag InputTag_LMB;
    FGameplayTag InputTag_RMB;
    FGameplayTag InputTag_1;
    FGameplayTag InputTag_2;
    FGameplayTag InputTag_3;
    FGameplayTag InputTag_4;
    FGameplayTag InputTag_5;
    FGameplayTag InputTag_6;

    // ===========================================
    // Damage Type Tags
    // ===========================================
    FGameplayTag DamageType_Physical;
    FGameplayTag DamageType_Magical;
    FGameplayTag DamageType_True;

private:
    static FF1GameplayTags GameplayTags;
};
