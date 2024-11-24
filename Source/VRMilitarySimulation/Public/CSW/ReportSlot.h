// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReportSlot.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UReportSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextNickname;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextKill;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextAccuracy;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextFeedback;

	UPROPERTY(meta=(BindWidget))
	class UImage* ImageGraph;

	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBoxFeedback;

	UFUNCTION()
	void OnClickFeedback(bool bIsChecked);

	void SetReportData(const FString& nickname, int32 kill, float accuracy, const FString& feedback);

	void SetRadarGraph(UTexture2D* texture);
};
