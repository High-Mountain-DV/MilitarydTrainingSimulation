// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReportWrapperWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UReportWrapperWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonGoHome;

	UPROPERTY(meta=(BindWidget))
	class UHorizontalBox* HorizontalBox;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* TextPlayTime;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextInjured;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextDead;
	
	UFUNCTION()
	void OnClickGoHome();

	void AddReportSlot(class UReportSlot* slot);

	void SetReportData(int32 playTime, int32 injured, int32 dead);
};
