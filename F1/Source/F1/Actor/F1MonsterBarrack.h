// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "F1MonsterBarrack.generated.h"

class AF1MonsterCharacter;
class USphereComponent;

UCLASS()
class F1_API AF1MonsterBarrack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AF1MonsterBarrack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 타이머에 의해 호출될 스폰 함수
	void SpawnMonster();

	// 배럭 주변에 몬스터가 몇 마리나 있는지 세는 함수
	int32 CountMonstersInRadius() const;

public:	
	// =========================================================
	// Components
	// =========================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBillboardComponent> SpriteComp;

	// 에디터에서 스폰 제한 범위를 시각적으로 보여줄 구체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> SpawnRangeSphere;

	// =========================================================
	// Settings
	// =========================================================
	UPROPERTY(EditAnywhere, Category = "Barrack Setting")
	TSubclassOf<AF1MonsterCharacter> MonsterClass;

	UPROPERTY(EditAnywhere, Category = "Barrack Setting")
	float SpawnInterval = 10.0f;

	// 이 반경 안에 몬스터가 많으면 스폰 중지
	UPROPERTY(EditAnywhere, Category = "Barrack Setting")
	float CheckRadius = 1500.0f;

	// 최대 유지 마리수 (이 숫자 이상이면 스폰 안 함)
	UPROPERTY(EditAnywhere, Category = "Barrack Setting")
	int32 MaxMonsterCount = 5;

private:
	FTimerHandle SpawnTimerHandle;
};
