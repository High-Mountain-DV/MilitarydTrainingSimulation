// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WaitRoomSlotWidget.h"
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
	class UScrollBox* ScrollBox;	

	UFUNCTION(BlueprintCallable)
	void OnClick_GoLobby();

	UFUNCTION(BlueprintCallable)
	void OnClick_GameStart();

	UFUNCTION(BlueprintCallable)
	void OnClick_GameReady();

	void AddPlayerPanel(const FString& nickname, bool bIsCommender);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UWaitRoomSlotWidget> WaitRoomSlotWidgetFactory;
	
private:
	UPROPERTY()
	TSet<AActor *> Players;
	
	int32 MaxPlayerCnt;
	FTimerHandle handle;

	const int32 ROLE = 0;
	const int32 READY = 1;
	const int32 NAME = 2;

};
