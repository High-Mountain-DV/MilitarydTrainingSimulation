// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ProfileReport.h"

#include "Components/TextBlock.h"
#include "CSW/GraphWidget.h"

void UProfileReport::NativeConstruct()
{
	Super::NativeConstruct();

	Graph_Kills->SetGraphColor(FLinearColor::Green);
	Graph_Accuracy->SetGraphColor(FLinearColor::Black);
	Graph_Assists->SetGraphColor(FLinearColor::Blue);
	Graph_Awareness->SetGraphColor(FLinearColor::Red);
	Graph_PlayTime->SetGraphColor(FLinearColor::Gray);
}

void UProfileReport::SetProfileReportData(const FProfileReportData& Report)
{
	int max_order = 5;

	const TArray<FGraphData>& Datas = Report.GraphData;
	for (int i = 0; i < Datas.Num(); i++)
	{
		int order = max_order - i;
		Graph_Kills->SetPoints(order, FMath::GetMappedRangeValueClamped(FVector2D(0, 10), FVector2D(0, 250), Datas[i].Kills));
		Graph_PlayTime->SetPoints(order, FMath::GetMappedRangeValueClamped(FVector2D(0, 3000), FVector2D(0, 250), Datas[i].PlayTime));
		Graph_Assists->SetPoints(order, FMath::GetMappedRangeValueClamped(FVector2D(0, 10), FVector2D(0, 250), Datas[i].Assists));
		Graph_Accuracy->SetPoints(order, FMath::GetMappedRangeValueClamped(FVector2D(0, 1), FVector2D(0, 250), Datas[i].Accuracy));
		Graph_Awareness->SetPoints(order, FMath::GetMappedRangeValueClamped(FVector2D(0, 10), FVector2D(0, 250),Datas[i].Awareness));
	}
	TextBlock_Feedback->SetText(FText::FromString(Report.LatestFeedback.Left(70)));
}
