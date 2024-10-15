// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaitRoomWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UWaitRoomWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Button_GoLobby;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_GameStart;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_GameReady;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Txt_Users;

	UFUNCTION()
	void OnClick_GoLobby();

	UFUNCTION()
	void OnClick_GameStart();

	UFUNCTION()
	void OnClick_GameReady();
};
