// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReportWidget.generated.h"

/**
 * 
 */
USTRUCT()
struct FReportData
{
	GENERATED_BODY()

	FString nickname;
	float playTime;
	float accuracy;
	int32 kill;
	int32 injuredPlayer;
	int32 deadPlayer;
	FString imageUrl;
	FString analysisResult;

	FReportData()
		: playTime(0), accuracy(0), kill(0), injuredPlayer(0), deadPlayer(0)
	{
	}
};

UCLASS()
class VRMILITARYSIMULATION_API UReportWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonGoLobby;

	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonReplay;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Text_Nickname;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_PlayTime;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_Accuracy;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_Kill;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_Injured;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* Text_Dead;

	UFUNCTION()
	void OnClickGoLobby();

	UFUNCTION()
	void OnClickReplay();

	void SetReportData(const FReportData& data);
};
