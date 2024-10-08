// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API USessionSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_RoomName;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_HostName;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_PlayerCount;

	UPROPERTY(meta=(BindWidget))
	class UButton* Button_Join;

	int32 SessionSearchIndex;

	UFUNCTION()
	void OnClickJoin();

	void UpdateInfo(const struct FRoomInfo& info);
};
