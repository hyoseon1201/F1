// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "F1GameMode.generated.h"

/**
 * 
 */
UCLASS()
class F1_API AF1GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	// 스폰 위치 고르기
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
	// 현재 접속한 플레이어 수
	int32 PlayerCount = 0;
};
