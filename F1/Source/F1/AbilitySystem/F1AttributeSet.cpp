// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/F1AttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/F1HeroCharacter.h"
#include "Game/F1PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include <Game/F1PlayerController.h>

UF1AttributeSet::UF1AttributeSet()
{
	CriticalStrikeDamage = 0.5f;
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
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, ArmorPenetrationFlat, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, ArmorPenetrationPercent, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MagicPenetrationFlat, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MagicPenetrationPercent, COND_None, REPNOTIFY_Always);

	// 흡혈
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, LifeSteal, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Omnivamp, COND_None, REPNOTIFY_Always);

	// 저항력
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Tenacity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, SlowResistance, COND_None, REPNOTIFY_Always);

	// 사거리
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, AttackRange, COND_None, REPNOTIFY_Always);

	// 캐릭터 정보 추가
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, CharacterLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, Experience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UF1AttributeSet, MaxExperience, COND_None, REPNOTIFY_Always);

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

	if (Data.EvaluatedData.Attribute == GetMovementSpeedAttribute())
	{
		if (ACharacter* Character = Cast<ACharacter>(Props.TargetAvatarActor))
		{
			if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
			{
				float NewSpeed = GetMovementSpeed();
				MovementComp->MaxWalkSpeed = NewSpeed;
			}
		}
	}

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0.f, GetMaxMana()));
	}

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			const bool bFatal = NewHealth <= 0.f;

			if (bFatal)
			{
				IF1CombatInterface* CombatInterface = Cast<IF1CombatInterface>(Props.TargetAvatarActor);
				if (CombatInterface)
				{
					CombatInterface->Die();
				}
			}

			ShowFloatingText(Props, LocalIncomingDamage);
		}
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

	if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
	{
		if (ASC->AbilityActorInfo.IsValid() && ASC->AbilityActorInfo->AvatarActor.IsValid())
		{
			if (ACharacter* Character = Cast<ACharacter>(ASC->AbilityActorInfo->AvatarActor.Get()))
			{
				if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
				{
					MovementComp->MaxWalkSpeed = GetMovementSpeed();
				}
			}
		}
	}
}

void UF1AttributeSet::OnRep_AbilityHaste(const FGameplayAttributeData& OldAbilityHaste) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, AbilityHaste, OldAbilityHaste);
}

void UF1AttributeSet::OnRep_ArmorPenetrationFlat(const FGameplayAttributeData& OldArmorPenetrationFlat) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, ArmorPenetrationFlat, OldArmorPenetrationFlat);
}

void UF1AttributeSet::OnRep_ArmorPenetrationPercent(const FGameplayAttributeData& OldArmorPenetrationPercent) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, ArmorPenetrationPercent, OldArmorPenetrationPercent);
}

void UF1AttributeSet::OnRep_MagicPenetrationFlat(const FGameplayAttributeData& OldMagicPenetrationFlat) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, MagicPenetrationFlat, OldMagicPenetrationFlat);
}

void UF1AttributeSet::OnRep_MagicPenetrationPercent(const FGameplayAttributeData& OldMagicPenetrationPercent) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, MagicPenetrationPercent, OldMagicPenetrationPercent);
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

void UF1AttributeSet::OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, CharacterLevel, OldCharacterLevel);
}

void UF1AttributeSet::OnRep_Experience(const FGameplayAttributeData& OldExperience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, Experience, OldExperience);
}

void UF1AttributeSet::OnRep_MaxExperience(const FGameplayAttributeData& OldMaxExperience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UF1AttributeSet, MaxExperience, OldMaxExperience);
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
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
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

void UF1AttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage) const
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		if (AF1PlayerController* PC = Cast<AF1PlayerController>(UGameplayStatics::GetPlayerController(Props.SourceCharacter, 0)))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter);
		}
	}
}
