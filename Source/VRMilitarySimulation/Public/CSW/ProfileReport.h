// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProfileReport.generated.h"

/**
 * 
 */

USTRUCT()
struct FProfileReportData
{
	GENERATED_BODY()

	FString date;
	int32 assists;
	int32 kills;
	float accuracy;
	float awareness;
	float playTime;
	FString aggregatedFeedback;

	FProfileReportData()
		: assists(0), kills(0), accuracy(0), awareness(0), playTime(0)
	{
	}
};

UCLASS()
class VRMILITARYSIMULATION_API UProfileReport : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UGraphWidget* Graph_Kills;

	UPROPERTY(meta=(BindWidget))
	class UGraphWidget* Graph_Assists;
	
	UPROPERTY(meta=(BindWidget))
	class UGraphWidget* Graph_Accuracy;

	UPROPERTY(meta=(BindWidget))
	class UGraphWidget* Graph_Awareness;

	UPROPERTY(meta=(BindWidget))
	class UGraphWidget* Graph_PlayTime;

	virtual void NativeConstruct() override;
	
	void SetProfileReportData(const TArray<FProfileReportData>& Datas);
};
