// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/CommenderScreenWidget.h"

#include "Components/Image.h"

void UCommenderScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Cams.Add(CamImage0);
	Cams.Add(CamImage1);
	Cams.Add(CamImage2);
	Cams.Add(CamImage3);
}

void UCommenderScreenWidget::AddPlayerScreen(UMaterialInstanceDynamic* CamMtl)
{
	if (Idx < 4)
		Cams[Idx++]->SetBrushFromMaterial(CamMtl);
}

void UCommenderScreenWidget::SelectScreen(int32 idx)
{
	WholeScreen->SetBrush(Cams[idx]->GetBrush());
	WholeScreen->SetVisibility(ESlateVisibility::Visible);
}

void UCommenderScreenWidget::UnselectScreen()
{
	WholeScreen->SetVisibility(ESlateVisibility::Collapsed);
}
