// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProfileWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UProfileWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;
	
	//===== menu ===========================
	UPROPERTY(meta=(BindWidget))
	class UButton* Menu_Button_GoInfo;

	UPROPERTY(meta=(BindWidget))
	UButton* Menu_Button_GoReport;

	UPROPERTY(meta=(BindWidget))
	UButton* Menu_Button_GoCustom;

	UPROPERTY(meta=(BindWidget))
	UButton* Menu_Button_Logout;	

	UFUNCTION()
	void Menu_OnClickGoInfo();

	UFUNCTION()
	void Menu_OnClickReport();

	UFUNCTION()
	void Menu_OnClickCustom();

	UFUNCTION()
	void Menu_OnClickLogout();

	// info =====================================
	UPROPERTY(meta=(BindWidget))
	UButton* Info_Button_GoBack;

	UFUNCTION()
	void Info_OnClickGoBack();

	// report ===================================
	UPROPERTY(meta=(BindWidget))
	UButton* Report_Button_GoBack;

	UPROPERTY(meta=(BindWidget))
	UButton* Report_Button_GoReplay;
	
	UFUNCTION()
	void Report_OnClickGoBack();

	UFUNCTION()
	void Report_OnClickGoReplay();

	// replay ===================================
	UPROPERTY(meta=(BindWidget))
	UButton* Replay_Button_GoBack;

	UFUNCTION()
	void Replay_OnClickGoBack();
	
	// custom ===================================
	UPROPERTY(meta=(BindWidget))
	UButton* Custom_Button_GoBack;

	UFUNCTION()
	void Custom_OnClickGoBack();
	
private:
	const int32 MENU_IDX = 0;
	const int32 INFO_IDX = 1;
	const int32 REPORT_IDX = 2;
	const int32 REPLAY_IDX = 3;
	const int32 CUSTOM_IDX = 4;


};
