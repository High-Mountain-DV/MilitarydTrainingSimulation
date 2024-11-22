// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/MissionLobbyWidget.h"

#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
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

	CR_Button_GoNext->OnClicked.AddDynamic(this , &UMissionLobbyWidget::CR_OnClickGoNext);
	CR_Button_GoMenu->OnClicked.AddDynamic(this, &UMissionLobbyWidget::CR_OnClickGoMenu);
	CR_Button_InputRoomName->OnClicked.AddDynamic(this, &UMissionLobbyWidget::CR_OnClickInputRoomName);

	RO_Button_CreateRoom->OnClicked.AddDynamic(this, &UMissionLobbyWidget::RO_OnClickCreateRoom);
	RO_Button_GoBack->OnClicked.AddDynamic(this, &UMissionLobbyWidget::RO_OnClickGoBack);
	
	FR_Button_GoMenu->OnClicked.AddDynamic(this, &UMissionLobbyWidget::FR_OnClickGoMenu);
}

void UMissionLobbyWidget::MENU_OnClickGoCreateRoom()
{
	WidgetSwitcher->SetActiveWidgetIndex(CR_IDX);
}

void UMissionLobbyWidget::MENU_OnClickGoFindSessions()
{
	WidgetSwitcher->SetActiveWidgetIndex(FR_IDX);

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

void UMissionLobbyWidget::CR_OnClickGoNext()
{
	WidgetSwitcher->SetActiveWidgetIndex(RO_IDX);
	
}

void UMissionLobbyWidget::CR_OnClickGoMenu()
{
	WidgetSwitcher->SetActiveWidgetIndex(MENU_IDX);
}

void UMissionLobbyWidget::CR_OnClickInputRoomName()
{
	SelectedInput = CR_Text_RoomName;

	SpawnKeyboardWidgetActor();
}

void UMissionLobbyWidget::RO_OnClickCreateRoom()
{
	auto* gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	FString roomName = CR_Text_RoomName->GetText().ToString();

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

void UMissionLobbyWidget::RO_OnClickGoBack()
{
	WidgetSwitcher->SetActiveWidgetIndex(CR_IDX);
}

void UMissionLobbyWidget::FR_OnClickGoMenu()
{
	FS_ScrollBox->ClearChildren();
	WidgetSwitcher->SetActiveWidgetIndex(MENU_IDX);
}

void UMissionLobbyWidget::AddSessionSlotWidget(const FRoomInfo& info)
{
	USessionSlotWidget* slot = CreateWidget<USessionSlotWidget>(this , SessionSlotWidgetFactory);

	if (slot)
	{
		slot->UpdateInfo(info);
		
		// FS_ScrollBox에 추가 하고싶다.
		FS_ScrollBox->AddChild(slot);
	}
}
