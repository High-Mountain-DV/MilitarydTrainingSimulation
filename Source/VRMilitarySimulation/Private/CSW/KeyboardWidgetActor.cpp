// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/KeyboardWidgetActor.h"

#include "Components/WidgetComponent.h"
#include "CSW/KeyboardWidget.h"
#include "CSW/VRWidget.h"

void AKeyboardWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	auto* keyboardWidget = Cast<UKeyboardWidget>(MainWidgetComp->GetWidget());
	if (keyboardWidget)
		keyboardWidget->SetKeyboardActor(this);
}

void AKeyboardWidgetActor::SetScreenWidget(UVRWidget* widget)
{
	ScreenWidget = widget;
}

void AKeyboardWidgetActor::PressKey(const FString& key)
{
	ScreenWidget->AppendSelectedInput(key);
	
}
