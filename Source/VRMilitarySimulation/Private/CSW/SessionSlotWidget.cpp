// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/SessionSlotWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CSW/CSWGameInstance.h"

void USessionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Join->OnClicked.AddDynamic(this , &USessionSlotWidget::OnClickJoin);
}

void USessionSlotWidget::OnClickJoin()
{
	// SessionSearchIndex
	auto* gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		gi->JoinMySession(SessionSearchIndex);
	}
}

void USessionSlotWidget::UpdateInfo(const struct FRoomInfo& info)
{
	Text_RoomName->SetText(FText::FromString(info.roomName));
	Text_HostName->SetText(FText::FromString(info.hostName));
	FString count = FString::Printf(TEXT("%d / %d") , info.currentPlayerCount, info.maxPlayerCount);
	Text_PlayerCount->SetText(FText::FromString(count));

	SessionSearchIndex = info.index;
}