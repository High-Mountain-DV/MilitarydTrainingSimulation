// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/LoginWidget.h"

#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/WidgetSwitcher.h"
#include "CSW/HttpLoginActor.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Lg_Button_Login->OnClicked.AddDynamic(this, &ULoginWidget::Lg_OnClickLogin);
	Lg_Button_GoRegister->OnClicked.AddDynamic(this, &ULoginWidget::Lg_OnClickGoRegister);
	Rg_Button_Register->OnClicked.AddDynamic(this, &ULoginWidget::Rg_OnClickRegister);
	Rg_Button_Cancel->OnClicked.AddDynamic(this, &ULoginWidget::Rg_OnClickCancel);

}

void ULoginWidget::Lg_OnClickLogin()
{
	HttpActor->RequestLogin(Lg_Input_Id->GetText().ToString(), Lg_Input_Passward->GetText().ToString());
}

void ULoginWidget::Lg_OnClickGoRegister()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void ULoginWidget::Rg_OnClickRegister()
{
	HttpActor->RequestRegister(Rg_Input_Id->GetText().ToString(), Rg_Input_Nickname->GetText().ToString(), Rg_Input_Passward->GetText().ToString());
}

void ULoginWidget::Rg_OnClickCancel()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
}

void ULoginWidget::SetHttpLoginActor(AHttpLoginActor* actor)
{
	HttpActor = actor;
}

