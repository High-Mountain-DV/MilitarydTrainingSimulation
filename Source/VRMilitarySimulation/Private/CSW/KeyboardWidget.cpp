// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/KeyboardWidget.h"

#include "CSW/KeyboardWidgetActor.h"

void UKeyboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UKeyboardWidget::PressKey(const FString& value)
{
	KeyboardActor->PressKey(value);
}

void UKeyboardWidget::SetKeyboardActor(AKeyboardWidgetActor* actor)
{
	KeyboardActor = actor;
}
