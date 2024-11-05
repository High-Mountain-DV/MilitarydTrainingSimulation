// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/KeyboardWidget.h"

#include "Components/Button.h"
#include "CSW/KeyboardWidgetActor.h"

void UKeyboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonClose->OnClicked.AddDynamic(this, &UKeyboardWidget::OnClickClose);
	ButtonEnter->OnClicked.AddDynamic(this, &UKeyboardWidget::OnClickEnter);
	ButtonBackSpace->OnClicked.AddDynamic(this, &UKeyboardWidget::OnClickBackSpace);
	ButtonLeftShift->OnClicked.AddDynamic(this, &UKeyboardWidget::OnClickShift);
	ButtonRightShift->OnClicked.AddDynamic(this, &UKeyboardWidget::OnClickShift);
	ButtonCapsLock->OnClicked.AddDynamic(this, &UKeyboardWidget::OnClickShift);
	ButtonSwitchLanguage->OnClicked.AddDynamic(this, &UKeyboardWidget::OnClickSwitchLanguage);
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

void UKeyboardWidget::OnClickBackSpace()
{
	KeyboardActor->PressBackSpace();
}


void UKeyboardWidget::OnClickSwitchLanguage()
{
	if (ToKoreanDelegate.IsBound())
		ToKoreanDelegate.Broadcast();
}

void UKeyboardWidget::OnClickShift()
{
	if (bUpper)
	{
		if (ToLowerDelegate.IsBound())
			ToLowerDelegate.Broadcast();
	}
	else
	{
		if (ToUpperDelegate.IsBound())
			ToUpperDelegate.Broadcast();
	}
	bUpper = !bUpper;
}

bool UKeyboardWidget::IsEnglish()
{
	return bEnglish;
}

bool UKeyboardWidget::IsUpper()
{
	return bUpper;
}
