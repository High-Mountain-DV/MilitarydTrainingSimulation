// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaitRoomSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UWaitRoomSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetNickname(const FString& nickname, bool isCommender);

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Nickname;

	UPROPERTY(meta=(BindWidget))
	class UImage* ImageCommender;	

	UPROPERTY(meta=(BindWidget))
	class UImage* ImageTrainee;
};
