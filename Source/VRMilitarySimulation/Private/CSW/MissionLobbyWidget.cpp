// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/MissionLobbyWidget.h"

#include <string>

#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "Components/WidgetSwitcher.h"
#include "CSW/SessionSlotWidget.h"
#include "CSW/CSWGameInstance.h"

void UMissionLobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto* gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	// OnSearchSignatureCompleteDelegate에 AddSessionSlotWidget을 연결하고싶다.
	gi->OnSearchSignatureCompleteDelegate.AddDynamic(this , &UMissionLobbyWidget::AddSessionSlotWidget);

	MENU_Button_GoCreateRoom->OnClicked.AddDynamic(this , &UMissionLobbyWidget::MENU_OnClickGoCreateRoom);
	MENU_Button_GoFindSessions->OnClicked.AddDynamic(this , &UMissionLobbyWidget::MENU_OnClickGoFindSessions);

	CR_Button_CreateRoom->OnClicked.AddDynamic(this , &UMissionLobbyWidget::CR_OnClickCreateRoom);
	CR_Button_GoMenu->OnClicked.AddDynamic(this, &UMissionLobbyWidget::CR_OnClickGoMenu);

	FR_Button_GoMenu->OnClicked.AddDynamic(this, &UMissionLobbyWidget::FR_OnClickGoMenu);

}

void UMissionLobbyWidget::MENU_OnClickGoCreateRoom()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void UMissionLobbyWidget::MENU_OnClickGoFindSessions()
{
	WidgetSwitcher->SetActiveWidgetIndex(2);

	auto* gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		UE_LOG(LogTemp, Warning, TEXT("Click"));

		gi->FindOtherSessions();
	}
}

// 무작위 문자열 생성 함수
FString UMissionLobbyWidget::GenerateRandomString(int32 Length)
{
	FString RandomString;
	FString Charset = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");

	for (int32 i = 0; i < Length; i++)
	{
		int32 Index = FMath::RandRange(0, Charset.Len() - 1);
		RandomString += Charset[Index];
	}

	return RandomString;
}

void UMissionLobbyWidget::CR_OnClickCreateRoom()
{
	auto* gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	FString roomName = CR_Edit_RoomName->GetText().ToString();

	// roomName이 기재되지 않거나 공백이라면 방생성을 하지 않고싶다.
	roomName = roomName.TrimStartAndEnd();
	if ( roomName.IsEmpty() )
	{
		int32 count = (int32)CR_Slider_PlayerCount->GetValue();
		gi->CreateMySession(GenerateRandomString(10) , count);
		return;
	}

	int32 count = (int32)CR_Slider_PlayerCount->GetValue();
	gi->CreateMySession(roomName , count);
}

void UMissionLobbyWidget::CR_OnClickGoMenu()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
}

void UMissionLobbyWidget::FR_OnClickGoMenu()
{
	FS_ScrollBox->ClearChildren();
	WidgetSwitcher->SetActiveWidgetIndex(0);

}

void UMissionLobbyWidget::AddSessionSlotWidget(const FRoomInfo& info)
{
	USessionSlotWidget* slot = CreateWidget<USessionSlotWidget>(this , SessionSlotWidgetFactory);
	
	slot->UpdateInfo(info);
	// FS_ScrollBox에 추가 하고싶다.

	FS_ScrollBox->AddChild(slot);
}
