// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/KeyboardWidget.h"

#include "Components/Button.h"
#include "CSW/KeyboardWidgetActor.h"

void UKeyboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonClose->OnClicked.AddDynamic(this, &UKeyboardWidget::OnClickClose);
	ButtonEnter->OnClicked.AddDynamic(this, &UKeyboardWidget::OnClickEnter);
}

void UKeyboardWidget::PressKey(const FString& value)
{
	KeyboardActor->PressKey(value);
}

void UKeyboardWidget::SetKeyboardActor(AKeyboardWidgetActor* actor)
{
	KeyboardActor = actor;
}

void UKeyboardWidget::OnClickClose()
{
	KeyboardActor->CloseKeyboard();
}

void UKeyboardWidget::OnClickEnter()
{
	KeyboardActor->CloseKeyboard();
}
