// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/F1UserWidget.h"

void UF1UserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
