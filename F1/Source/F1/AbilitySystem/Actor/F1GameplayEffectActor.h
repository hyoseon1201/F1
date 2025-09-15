// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "F1GameplayEffectActor.generated.h"

class USphereComponent;
class UGameplayEffect;

UCLASS()
class F1_API AF1GameplayEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AF1GameplayEffectActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	float EffectLevel = 1.0f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);
};
