// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ReportWidget.h"

#include "Components/Button.h"
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
