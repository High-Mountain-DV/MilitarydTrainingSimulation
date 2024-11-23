// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProfileReport.generated.h"

/**
 * 
 */

USTRUCT()
struct FGraphData
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString Date;

	UPROPERTY()
	int32 Kills;

	UPROPERTY()
	float Awareness;

	UPROPERTY()
	int32 Assists;

	UPROPERTY()
	float Accuracy;

	UPROPERTY()
	int32 PlayTime;
};

USTRUCT(BlueprintType)
struct FLatestFeedback
{
	GENERATED_BODY()

	UPROPERTY()
	FString Feedback;
};

USTRUCT(BlueprintType)
struct FProfileReportData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FGraphData> GraphData;

	UPROPERTY()
	FLatestFeedback LatestFeedback;
	
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
	
	void SetProfileReportData(const FProfileReportData& Report);
};
