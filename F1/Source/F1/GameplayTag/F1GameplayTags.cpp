#include "F1GameplayTags.h"
#include "GameplayTagsManager.h"

FF1GameplayTags FF1GameplayTags::GameplayTags;

void FF1GameplayTags::InitializeNativeGameplayTags()
{
    UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

    // ===========================================
    // Attribute Tags - Vital Stats
    // ===========================================
    GameplayTags.Attributes_Vital_Health = Manager.AddNativeGameplayTag(
        FName("Attributes.Vital.Health"),
        FString("Current Health of the character")
    );

    GameplayTags.Attributes_Vital_MaxHealth = Manager.AddNativeGameplayTag(
        FName("Attributes.Vital.MaxHealth"),
        FString("Maximum Health of the character")
    );

    GameplayTags.Attributes_Vital_HealthRegeneration = Manager.AddNativeGameplayTag(
        FName("Attributes.Vital.HealthRegeneration"),
        FString("Health regeneration rate per second")
    );

    GameplayTags.Attributes_Vital_Mana = Manager.AddNativeGameplayTag(
        FName("Attributes.Vital.Mana"),
        FString("Current Mana of the character")
    );

    GameplayTags.Attributes_Vital_MaxMana = Manager.AddNativeGameplayTag(
        FName("Attributes.Vital.MaxMana"),
        FString("Maximum Mana of the character")
    );

    GameplayTags.Attributes_Vital_ManaRegeneration = Manager.AddNativeGameplayTag(
        FName("Attributes.Vital.ManaRegeneration"),
        FString("Mana regeneration rate per second")
    );

    // ===========================================
    // Attribute Tags - Offensive Stats
    // ===========================================
    GameplayTags.Attributes_Offensive_AttackDamage = Manager.AddNativeGameplayTag(
        FName("Attributes.Offensive.AttackDamage"),
        FString("Physical attack damage")
    );

    GameplayTags.Attributes_Offensive_AttackSpeed = Manager.AddNativeGameplayTag(
        FName("Attributes.Offensive.AttackSpeed"),
        FString("Attack speed multiplier")
    );

    GameplayTags.Attributes_Offensive_AbilityPower = Manager.AddNativeGameplayTag(
        FName("Attributes.Offensive.AbilityPower"),
        FString("Magical ability power")
    );

    GameplayTags.Attributes_Offensive_CriticalStrikeChance = Manager.AddNativeGameplayTag(
        FName("Attributes.Offensive.CriticalStrikeChance"),
        FString("Critical strike chance percentage")
    );

    GameplayTags.Attributes_Offensive_CriticalStrikeDamage = Manager.AddNativeGameplayTag(
        FName("Attributes.Offensive.CriticalStrikeDamage"),
        FString("Critical strike damage multiplier")
    );

    // ===========================================
    // Attribute Tags - Defensive Stats
    // ===========================================
    GameplayTags.Attributes_Defensive_Armor = Manager.AddNativeGameplayTag(
        FName("Attributes.Defensive.Armor"),
        FString("Physical damage resistance")
    );

    GameplayTags.Attributes_Defensive_MagicResistance = Manager.AddNativeGameplayTag(
        FName("Attributes.Defensive.MagicResistance"),
        FString("Magical damage resistance")
    );

    // ===========================================
    // Attribute Tags - Movement & Utility
    // ===========================================
    GameplayTags.Attributes_Movement_Speed = Manager.AddNativeGameplayTag(
        FName("Attributes.Movement.Speed"),
        FString("Character movement speed")
    );

    GameplayTags.Attributes_Utility_AbilityHaste = Manager.AddNativeGameplayTag(
        FName("Attributes.Utility.AbilityHaste"),
        FString("Cooldown reduction percentage")
    );

    // ===========================================
    // Attribute Tags - Penetration
    // ===========================================
    GameplayTags.Attributes_Penetration_Armor = Manager.AddNativeGameplayTag(
        FName("Attributes.Penetration.Armor"),
        FString("Armor penetration amount")
    );

    GameplayTags.Attributes_Penetration_Magic = Manager.AddNativeGameplayTag(
        FName("Attributes.Penetration.Magic"),
        FString("Magic penetration amount")
    );

    // ===========================================
    // Attribute Tags - Sustain
    // ===========================================
    GameplayTags.Attributes_Sustain_LifeSteal = Manager.AddNativeGameplayTag(
        FName("Attributes.Sustain.LifeSteal"),
        FString("Physical damage life steal percentage")
    );

    GameplayTags.Attributes_Sustain_Omnivamp = Manager.AddNativeGameplayTag(
        FName("Attributes.Sustain.Omnivamp"),
        FString("All damage life steal percentage")
    );

    // ===========================================
    // Attribute Tags - Resistance
    // ===========================================
    GameplayTags.Attributes_Resistance_Tenacity = Manager.AddNativeGameplayTag(
        FName("Attributes.Resistance.Tenacity"),
        FString("Crowd control duration reduction")
    );

    GameplayTags.Attributes_Resistance_SlowResistance = Manager.AddNativeGameplayTag(
        FName("Attributes.Resistance.SlowResistance"),
        FString("Movement speed reduction resistance")
    );

    // ===========================================
    // Attribute Tags - Range
    // ===========================================
    GameplayTags.Attributes_Range_Attack = Manager.AddNativeGameplayTag(
        FName("Attributes.Range.Attack"),
        FString("Basic attack range")
    );

    // ===========================================
    // State Tags (MOBA 게임용 추가)
    // ===========================================
    GameplayTags.State_Dead = Manager.AddNativeGameplayTag(
        FName("State.Dead"),
        FString("Character is dead")
    );

    GameplayTags.State_Stunned = Manager.AddNativeGameplayTag(
        FName("State.Stunned"),
        FString("Character is stunned")
    );

    GameplayTags.State_Silenced = Manager.AddNativeGameplayTag(
        FName("State.Silenced"),
        FString("Character is silenced")
    );

    GameplayTags.State_Rooted = Manager.AddNativeGameplayTag(
        FName("State.Rooted"),
        FString("Character is rooted")
    );

    GameplayTags.State_Invulnerable = Manager.AddNativeGameplayTag(
        FName("State.Invulnerable"),
        FString("Character is invulnerable to damage")
    );

    // ===========================================
    // Ability Tags
    // ===========================================
    GameplayTags.Ability_Q = Manager.AddNativeGameplayTag(
        FName("Ability.Q"),
        FString("Q ability")
    );

    GameplayTags.Ability_W = Manager.AddNativeGameplayTag(
        FName("Ability.W"),
        FString("W ability")
    );

    GameplayTags.Ability_E = Manager.AddNativeGameplayTag(
        FName("Ability.E"),
        FString("E ability")
    );

    GameplayTags.Ability_R = Manager.AddNativeGameplayTag(
        FName("Ability.R"),
        FString("Ultimate ability")
    );

    GameplayTags.Ability_D = Manager.AddNativeGameplayTag(
        FName("Ability.D"),
        FString("Summoner Spell ability")
    );

    GameplayTags.Ability_F = Manager.AddNativeGameplayTag(
        FName("Ability.F"),
        FString("Summoner Spell ability")
    );

    GameplayTags.Ability_BasicAttack = Manager.AddNativeGameplayTag(
        FName("Ability.BasicAttack"),
        FString("Basic attack ability")
    );

    // ===========================================
    // Input Tags
    // ===========================================
    GameplayTags.InputTag_Q = Manager.AddNativeGameplayTag(
        FName("InputTag.Q"),
        FString("Q key input")
    );

    GameplayTags.InputTag_W = Manager.AddNativeGameplayTag(
        FName("InputTag.W"),
        FString("W key input")
    );

    GameplayTags.InputTag_E = Manager.AddNativeGameplayTag(
        FName("InputTag.E"),
        FString("E key input")
    );

    GameplayTags.InputTag_R = Manager.AddNativeGameplayTag(
        FName("InputTag.R"),
        FString("R key input")
    );

    GameplayTags.InputTag_D = Manager.AddNativeGameplayTag(
        FName("InputTag.D"),
        FString("D key input")
    );

    GameplayTags.InputTag_F = Manager.AddNativeGameplayTag(
        FName("InputTag.F"),
        FString("F key input")
    );

    GameplayTags.InputTag_BasicAttack = Manager.AddNativeGameplayTag(
        FName("InputTag.BasicAttack"),
        FString("Basic attack input")
    );

    GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.LMB"),
        FString("Input Tag for Left Mouse Button")
    );

    GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.RMB"),
        FString("Input Tag for Right Mouse Button")
    );

    GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.1"),
        FString("Input Tag for 1 key")
    );

    GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.2"),
        FString("Input Tag for 2 key")
    );

    GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.3"),
        FString("Input Tag for 3 key")
    );

    GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.4"),
        FString("Input Tag for 4 key")
    );

    GameplayTags.InputTag_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.5"),
        FString("Input Tag for 5 key")
    );

    GameplayTags.InputTag_6 = UGameplayTagsManager::Get().AddNativeGameplayTag(
        FName("InputTag.6"),
        FString("Input Tag for 6 key")
    );

    // ===========================================
    // Damage Type Tags
    // ===========================================
    GameplayTags.DamageType_Physical = Manager.AddNativeGameplayTag(
        FName("DamageType.Physical"),
        FString("Physical damage type")
    );

    GameplayTags.DamageType_Magical = Manager.AddNativeGameplayTag(
        FName("DamageType.Magical"),
        FString("Magical damage type")
    );

    GameplayTags.DamageType_True = Manager.AddNativeGameplayTag(
        FName("DamageType.True"),
        FString("True damage type")
    );

    // ===========================================
    // Damage Tags (SetByCaller용)
    // ===========================================
    GameplayTags.Damage_Basic = Manager.AddNativeGameplayTag(
        FName("Damage.Basic"),
        FString("Basic Attack Damage"));

    GameplayTags.Damage_Skill_Q = Manager.AddNativeGameplayTag(
        FName("Damage.Skill.Q"),
        FString("Q Skill Damage"));

    GameplayTags.Damage_Skill_W = Manager.AddNativeGameplayTag(
        FName("Damage.Skill.W"),
        FString("W Skill Damage"));

    GameplayTags.Damage_Skill_E = Manager.AddNativeGameplayTag(
        FName("Damage.Skill.E"),
        FString("E Skill Damage"));

    GameplayTags.Damage_Skill_R = Manager.AddNativeGameplayTag(
        FName("Damage.Skill.R"),
        FString("R Skill Ultimate Damage"));

    GameplayTags.Damage_DOT = Manager.AddNativeGameplayTag(
        FName("Damage.DOT"),
        FString("Damage Over Time"));

    GameplayTags.Damage_Execute = Manager.AddNativeGameplayTag(
        FName("Damage.Execute"),
        FString("Execute Damage"));
}
