// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/F1HUD.h"
#include "UI/Widget/F1UserWidget.h"

void AF1HUD::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	Widget->AddToViewport();
}
