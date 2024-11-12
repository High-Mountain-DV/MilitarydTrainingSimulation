// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ReportWrapperWidget.h"

#include "CSW/ReportWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UReportWrapperWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonGoLeft->OnClicked.AddDynamic(this, &UReportWrapperWidget::OnClickGoLeft);
	ButtonGoRight->OnClicked.AddDynamic(this, &UReportWrapperWidget::OnClickGoRight);
}

void UReportWrapperWidget::OnClickGoLeft()
{
	int32 len = WidgetSwitcher->GetChildrenCount();
	int32 idx = WidgetSwitcher->GetActiveWidgetIndex();

	if (len)
	{
		WidgetSwitcher->SetActiveWidgetIndex(--idx < 0 ? len - 1 : idx);
	}
}

void UReportWrapperWidget::OnClickGoRight()
{
	int32 len = WidgetSwitcher->GetChildrenCount();
	int32 idx = WidgetSwitcher->GetActiveWidgetIndex();

	if (len)
	{
		WidgetSwitcher->SetActiveWidgetIndex(++idx % len);
	}
}

void UReportWrapperWidget::AppendReport(UReportWidget* Report)
{
	WidgetSwitcher->AddChild(Report);
	if (WidgetSwitcher->GetChildrenCount() > 1)
	{
		ButtonGoLeft->SetVisibility(ESlateVisibility::Visible);
		ButtonGoRight->SetVisibility(ESlateVisibility::Visible);
	}
}
