// Fill out your copyright notice in the Description page of Project Settings.


#include "System/F1AssetManager.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTag/F1GameplayTags.h"

UF1AssetManager& UF1AssetManager::Get()
{
	check(GEngine);
	UF1AssetManager* F1AssetManager = Cast<UF1AssetManager>(GEngine->AssetManager);
	return *F1AssetManager;
}

void UF1AssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FF1GameplayTags::InitializeNativeGameplayTags();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
