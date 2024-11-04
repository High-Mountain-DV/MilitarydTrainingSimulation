// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/KeyboardButtonWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CSW/KeyboardWidget.h"

void UKeyboardButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button->OnClicked.AddDynamic(this, &UKeyboardButtonWidget::OnClickButton);
}

void UKeyboardButtonWidget::OnClickButton()
{
	UKeyboardWidget* KeyboardWidget = Cast<UKeyboardWidget>(GetOuter()->GetOuter());
	
	if (KeyboardWidget)
	{
		KeyboardWidget->PressKey(Value->GetText().ToString());
	}
}
