// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRWidget.h"
#include "Blueprint/UserWidget.h"
#include "MissionLobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UMissionLobbyWidget : public UVRWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;
	
	// Menu ===============================================
	UPROPERTY(meta=(BindWidget))
	class UButton* MENU_Button_GoCreateRoom;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* MENU_Button_GoFindSessions;

	// UPROPERTY(meta=(BindWidget))
	// class UEditableText* MENU_Edit_SessionName;

	UFUNCTION()
	void MENU_OnClickGoCreateRoom();

	UFUNCTION()
	void MENU_OnClickGoFindSessions();
	FString GenerateRandomString(int32 Length);

	// Create Room=========================================
	UFUNCTION()
	void CR_OnClickGoNext();

	UFUNCTION()
	void CR_OnClickGoMenu();

	UFUNCTION()
	void CR_OnClickInputRoomName();

	UPROPERTY(meta=(BindWidget))
	class UButton* CR_Button_GoNext;

	UPROPERTY(meta=(BindWidget))
	class UButton* CR_Button_InputRoomName;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CR_Text_RoomName;

	UPROPERTY(meta=(BindWidget))
	class USlider* CR_Slider_PlayerCount;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CR_Text_PlayerCount;

	UPROPERTY(meta=(BindWidget))
	class UButton* CR_Button_GoMenu;

	// Room Option ===========================
	UFUNCTION()
	void RO_OnClickCreateRoom();

	UFUNCTION()
	void RO_OnClickGoBack();

	UPROPERTY(meta=(BindWidget))
	UButton* RO_Button_GoBack;

	UPROPERTY(meta=(BindWidget))
	UButton* RO_Button_CreateRoom;

	
	// Find Session===================================
	UFUNCTION()
	void FR_OnClickGoMenu();

	UFUNCTION()
	void AddSessionSlotWidget(const struct FRoomInfo& info);

	UPROPERTY(meta=(BindWidget))
	class UScrollBox* FS_ScrollBox;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class USessionSlotWidget> SessionSlotWidgetFactory;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* FR_Button_GoMenu;

private:
	const int32 MENU_IDX = 0;
	const int32 CR_IDX = 1;
	const int32 RO_IDX = 2;
	const int32 FR_IDX = 3;
};
