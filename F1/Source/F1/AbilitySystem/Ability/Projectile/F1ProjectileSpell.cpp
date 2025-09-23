// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/Projectile/F1ProjectileSpell.h"
#include "AbilitySystem/Actor/F1Projectile.h"
#include <Interaction/F1CombatInterface.h>

void UF1ProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const bool bIsServer = HasAuthority(&ActivationInfo);
	if (!bIsServer) return;

	IF1CombatInterface* F1CombatInterface = Cast<IF1CombatInterface>(GetAvatarActorFromActorInfo());

	if (F1CombatInterface)
	{
		const FVector SocketLocation = F1CombatInterface->GetCombatSocketLocation();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		// TODO : 로테이션 설정

		AF1Projectile* Projectile = GetWorld()->SpawnActorDeferred<AF1Projectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		// TODO : 프로젝타일에 GE 부여

		Projectile->FinishSpawning(SpawnTransform);
	}
}
