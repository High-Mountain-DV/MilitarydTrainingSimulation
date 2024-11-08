// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ReportWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CSW/CSWGameInstance.h"

void UReportWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ButtonGoLobby->OnClicked.AddDynamic(this, &UReportWidget::OnClickGoLobby);
	ButtonReplay->OnClicked.AddDynamic(this, &UReportWidget::OnClickReplay);
}

void UReportWidget::OnClickGoLobby()
{
	auto gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());

	if (gi)
	{
		gi->GoLobby();
	}
}

void UReportWidget::OnClickReplay()
{
	auto gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());

	if (gi)
	{
		gi->ReplayRecording();
	}
}

void UReportWidget::SetReportData(const FReportData& data)
{
	Text_Nickname->SetText(FText::FromString(data.nickname));
	Text_PlayTime->SetText(FText::FromString(FString::SanitizeFloat(data.playTime)));
	Text_Accuracy->SetText(FText::FromString(FString::SanitizeFloat(data.playTime)));
	Text_Kill->SetText(FText::FromString(FString::FromInt(data.kill)));
	Text_Injured->SetText(FText::FromString(FString::FromInt(data.injuredPlayer)));
	Text_Dead->SetText(FText::FromString(FString::FromInt(data.injuredPlayer)));
}
