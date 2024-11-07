// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ProfileWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UProfileWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//menu
	Menu_Button_GoInfo->OnClicked.AddDynamic(this, &UProfileWidget::Menu_OnClickGoInfo);
	Menu_Button_GoReport->OnClicked.AddDynamic(this, &UProfileWidget::Menu_OnClickReport);
	Menu_Button_GoCustom->OnClicked.AddDynamic(this, &UProfileWidget::Menu_OnClickCustom);
	Menu_Button_Logout->OnClicked.AddDynamic(this, &UProfileWidget::Menu_OnClickLogout);

	// info
	Info_Button_GoBack->OnClicked.AddDynamic(this, &UProfileWidget::Info_OnClickGoBack);
	
	// report
	Report_Button_GoBack->OnClicked.AddDynamic(this, &UProfileWidget::Report_OnClickGoBack);
	Report_Button_GoReplay->OnClicked.AddDynamic(this, &UProfileWidget::Report_OnClickGoReplay);

	// replay
	Replay_Button_GoBack->OnClicked.AddDynamic(this, &UProfileWidget::Replay_OnClickGoBack);

	// custom
	Custom_Button_GoBack->OnClicked.AddDynamic(this, &UProfileWidget::Custom_OnClickGoBack);
}

void UProfileWidget::Menu_OnClickGoInfo()
{
	WidgetSwitcher->SetActiveWidgetIndex(INFO_IDX);
}

void UProfileWidget::Menu_OnClickReport()
{
	WidgetSwitcher->SetActiveWidgetIndex(REPORT_IDX);
}

void UProfileWidget::Menu_OnClickCustom()
{
	WidgetSwitcher->SetActiveWidgetIndex(CUSTOM_IDX);
}

void UProfileWidget::Menu_OnClickLogout()
{
}

void UProfileWidget::Info_OnClickGoBack()
{
	WidgetSwitcher->SetActiveWidgetIndex(MENU_IDX);
}

void UProfileWidget::Report_OnClickGoBack()
{
	WidgetSwitcher->SetActiveWidgetIndex(MENU_IDX);
}

void UProfileWidget::Report_OnClickGoReplay()
{
	WidgetSwitcher->SetActiveWidgetIndex(REPLAY_IDX);
}

void UProfileWidget::Replay_OnClickGoBack()
{
	WidgetSwitcher->SetActiveWidgetIndex(REPORT_IDX);
}

void UProfileWidget::Custom_OnClickGoBack()
{
	WidgetSwitcher->SetActiveWidgetIndex(MENU_IDX);
}
