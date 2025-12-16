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
    // Ability Tags (스킬의 신원 - 구체적으로!)
    // ===========================================
    // [공통]
    FGameplayTag Ability_Attack_Melee; // 근접 평타
    FGameplayTag Ability_Attack_Ranged; // 원거리 평타

    // [Dekker Skill]
    FGameplayTag Ability_Skill_ArcBolt;    // Q
    FGameplayTag Ability_Skill_Haste;   // W
    FGameplayTag Ability_Skill_SpawnDron;  // E
    FGameplayTag Ability_Skill_MeteorShower; // R

    // [Crunch Skill]
    FGameplayTag Ability_Skill_FistBarrage;  
    FGameplayTag Ability_Skill_Uppercut;     
    FGameplayTag Ability_Skill_RocketDash;   
    FGameplayTag Ability_Skill_Overdrive;

    // 공격중 태그
    FGameplayTag Ability_State_Attacking;
    FGameplayTag Ability_State_Casting;

    // ===========================================
    // Cooldown Tags (추가 필수! Ability 태그와 1:1 대응)
    // ===========================================
    FGameplayTag Cooldown_Attack; // [핵심] 평타 쿨타임용 (공속 제한에 쓰임)

    FGameplayTag Cooldown_Skill_ArcBolt;
    FGameplayTag Cooldown_Skill_Haste;
    FGameplayTag Cooldown_Skill_SpawnDron;
    FGameplayTag Cooldown_Skill_MeteorShower;

    FGameplayTag Cooldown_Skill_FistBarrage;
    FGameplayTag Cooldown_Skill_Uppercut;
    FGameplayTag Cooldown_Skill_RocketDash;
    FGameplayTag Cooldown_Skill_Overdrive;

    // ===========================================
    // Event Tags
    // ===========================================
    // PC에서 자동으로 돌릴 기본공격용 태그
    FGameplayTag Event_Combat_Attack;

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
    // Damage Type Tags (SetByCaller용)
    // ===========================================
    FGameplayTag DamageType_Physical;
    FGameplayTag DamageType_Magical;
    FGameplayTag DamageType_True;

    // ===========================================
    // Reward Tags (SetByCaller용)
    // ===========================================
    FGameplayTag Attributes_Meta_Experience;
    FGameplayTag Attributes_Meta_Gold;

    // ===========================================
    // CharacterType
    // ===========================================
    FGameplayTag CharacterType_Hero;
    FGameplayTag CharacterType_Monster;

private:
    static FF1GameplayTags GameplayTags;
};
