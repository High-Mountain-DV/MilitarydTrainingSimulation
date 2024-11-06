// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/LoginWidget.h"

#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "CSW/HttpLoginActor.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Lg_Button_InputId->OnClicked.AddDynamic(this, &ULoginWidget::Lg_OnClickInputId);
	Lg_Button_InputPassward->OnClicked.AddDynamic(this, &ULoginWidget::Lg_OnClickInputPassward);
	
	Lg_Button_Login->OnClicked.AddDynamic(this, &ULoginWidget::Lg_OnClickLogin);
	Lg_Button_GoRegister->OnClicked.AddDynamic(this, &ULoginWidget::Lg_OnClickGoRegister);

	Rg_Button_Register->OnClicked.AddDynamic(this, &ULoginWidget::Rg_OnClickRegister);
	Rg_Button_Cancel->OnClicked.AddDynamic(this, &ULoginWidget::Rg_OnClickCancel);

	Rg_Button_InputId->OnClicked.AddDynamic(this, &ULoginWidget::Rg_OnClickInputId);
	Rg_Button_InputNickname->OnClicked.AddDynamic(this, &ULoginWidget::Rg_OnClickInputNickname);
	Rg_Button_InputPassward->OnClicked.AddDynamic(this, &ULoginWidget::Rg_OnClickInputPassward);
}

void ULoginWidget::Lg_OnClickLogin()
{
	HttpActor->RequestLogin(Lg_Text_Id->GetText().ToString(), Lg_Text_Id->GetText().ToString());
}

void ULoginWidget::Lg_OnClickGoRegister()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void ULoginWidget::Lg_OnClickInputId()
{
	SelectedInput = Lg_Text_Id;

	SpawnKeyboardWidgetActor();
}

void ULoginWidget::Lg_OnClickInputPassward()
{
	SelectedInput = Lg_Text_Passward;

	SpawnKeyboardWidgetActor();
}

void ULoginWidget::Rg_OnClickRegister()
{
	HttpActor->RequestRegister(Rg_Text_Id->GetText().ToString(), Rg_Text_Nickname->GetText().ToString(), Rg_Text_Passward->GetText().ToString());
}

void ULoginWidget::Rg_OnClickCancel()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
}

void ULoginWidget::Rg_OnClickInputId()
{
	SelectedInput = Rg_Text_Id;

	SpawnKeyboardWidgetActor();

}

void ULoginWidget::Rg_OnClickInputNickname()
{
	SelectedInput = Rg_Text_Nickname;

	SpawnKeyboardWidgetActor();
}

void ULoginWidget::Rg_OnClickInputPassward()
{
	SelectedInput = Rg_Text_Passward;

	SpawnKeyboardWidgetActor();
}

void ULoginWidget::SetHttpLoginActor(AHttpLoginActor* actor)
{
	HttpActor = actor;
}

