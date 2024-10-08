// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MissionLobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UMissionLobbyWidget : public UUserWidget
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

	// Create Room=========================================
	UFUNCTION()
	void CR_OnClickCreateRoom();

	UFUNCTION()
	void CR_OnClickGoMenu();

	UPROPERTY(meta=(BindWidget))
	class UButton* CR_Button_CreateRoom;

	UPROPERTY(meta=(BindWidget))
	class UEditableText* CR_Edit_RoomName;

	UPROPERTY(meta=(BindWidget))
	class USlider* CR_Slider_PlayerCount;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CR_Text_PlayerCount;

	UPROPERTY(meta=(BindWidget))
	class UButton* CR_Button_GoMenu;

	
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
};
