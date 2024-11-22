// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ProfileReport.h"

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

void UProfileReport::SetProfileReportData(const TArray<FProfileReportData>& Datas)
{
	int max_order = 5;
	
	for (int i = 0; i < Datas.Num(); i++)
	{
		int order = max_order - Datas.Num() + 1;
		Graph_Kills->SetPoints(order, Datas[i].kills * 10);
		Graph_PlayTime->SetPoints(order, Datas[i].playTime);
		Graph_Assists->SetPoints(order, Datas[i].assists * 10);
		Graph_Accuracy->SetPoints(order, Datas[i].accuracy * 10);
		Graph_Awareness->SetPoints(order, Datas[i].awareness * 10);
	}
}
