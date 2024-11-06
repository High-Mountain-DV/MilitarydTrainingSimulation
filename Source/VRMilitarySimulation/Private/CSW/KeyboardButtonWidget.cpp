// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/KeyboardButtonWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CSW/KeyboardWidget.h"

void UKeyboardButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	KeyboardWidget = Cast<UKeyboardWidget>(GetOuter()->GetOuter());
	if (KeyboardWidget)
	{
		KeyboardWidget->ToUpperDelegate.AddDynamic(this, &UKeyboardButtonWidget::ToUpper);
		KeyboardWidget->ToLowerDelegate.AddDynamic(this, &UKeyboardButtonWidget::ToLower);
		// KeyboardWidget->ToKoreanDelegate.AddDynamic(this, &UKeyboardButtonWidget::ToKorean);
	}
	Button->OnClicked.AddDynamic(this, &UKeyboardButtonWidget::OnClickButton);
}

void UKeyboardButtonWidget::OnClickButton()
{
	if (KeyboardWidget)
	{
		KeyboardWidget->PressKey(Value->GetText().ToString());
	}
}

void UKeyboardButtonWidget::ToKorean()
{
	if (!KoValue.IsEmpty())
	{
		Value->SetText(FText::FromString(KoValue));
	}
}

void UKeyboardButtonWidget::ToUpper()
{
	if (!EnValue.IsEmpty() && 'a' <= EnValue[0] && EnValue[0] <= 'z')
	{
		EnValue[0] += 'A' - 'a';
		Value->SetText(FText::FromString(EnValue));
	}
}

void UKeyboardButtonWidget::ToLower()
{
	if (!EnValue.IsEmpty() && 'A' <= EnValue[0] && EnValue[0] <= 'Z')
	{
		EnValue[0] += 'a' - 'A';
		Value->SetText(FText::FromString(EnValue));
	}
}
