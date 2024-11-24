// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ReportSlot.h"

#include "Components/CheckBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UReportSlot::NativeConstruct()
{
	Super::NativeConstruct();

	CheckBoxFeedback->OnCheckStateChanged.AddDynamic(this, &UReportSlot::OnClickFeedback);
}

void UReportSlot::OnClickFeedback(bool bIsChecked)
{
	if (bIsChecked)
	{
		TextFeedback->SetVisibility(ESlateVisibility::Visible);	
	}
	else
	{
		TextFeedback->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UReportSlot::SetReportData(const FString& nickname, int32 kill, float accuracy,
	const FString& feedback)
{
	TextNickname->SetText(FText::FromString(nickname));
	TextKill->SetText(FText::FromString(FString::FromInt(kill)));
	TextAccuracy->SetText(FText::FromString(FString::SanitizeFloat(accuracy)));
	TextFeedback->SetText(FText::FromString(feedback));
}

void UReportSlot::SetRadarGraph(UTexture2D* texture)
{
	ImageGraph->SetBrushFromTexture(texture);
}
