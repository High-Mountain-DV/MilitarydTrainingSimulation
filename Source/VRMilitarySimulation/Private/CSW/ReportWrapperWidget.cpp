// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ReportWrapperWidget.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "CSW/ReportWidget.h"
#include "CSW/CSWGameInstance.h"
#include "CSW/ReportSlot.h"

void UReportWrapperWidget::NativeConstruct()
{
	ButtonGoHome->OnClicked.AddDynamic(this, &UReportWrapperWidget::OnClickGoHome);
}

void UReportWrapperWidget::OnClickGoHome()
{
	auto gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());

	if (gi)
	{
		gi->ResetTraineesId();
		gi->GoLobby();
	}
}

void UReportWrapperWidget::AddReportSlot(UReportSlot* slot)
{
	HorizontalBox->AddChild(slot);
}

void UReportWrapperWidget::SetReportData(int32 playTime, int32 injured, int32 dead)
{
	TextPlayTime->SetText(FText::FromString(FString::FromInt(playTime)));
	TextInjured->SetText(FText::FromString(FString::FromInt(injured)));
	TextDead->SetText(FText::FromString(FString::FromInt(dead)));
}
