// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/F1CharacterBase.h"
#include "Components/SkeletalMeshComponent.h"

AF1CharacterBase::AF1CharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AF1CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}
