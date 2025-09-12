// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Actor/F1GameplayEffectActor.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/F1AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

AF1GameplayEffectActor::AF1GameplayEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
}

void AF1GameplayEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (InstantGameplayEffectClass && OtherActor)
    {
        if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
        {
            UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent();
            if (TargetASC)
            {
                FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
                EffectContext.AddSourceObject(this);

                FGameplayEffectSpecHandle EffectSpec = TargetASC->MakeOutgoingSpec(
                    InstantGameplayEffectClass,
                    1.0f,  // Level
                    EffectContext
                );

                if (EffectSpec.IsValid())
                {
                    TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpec.Data.Get());
                    Destroy();
                }
            }
        }
    }
}


void AF1GameplayEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AF1GameplayEffectActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AF1GameplayEffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AF1GameplayEffectActor::EndOverlap);
}
