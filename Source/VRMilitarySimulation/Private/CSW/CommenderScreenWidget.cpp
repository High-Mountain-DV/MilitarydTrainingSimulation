// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/CommenderScreenWidget.h"

#include "Components/Image.h"

void UCommenderScreenWidget::AddPlayerScreen(UMaterialInstanceDynamic* CamMtl)
{
	CamImage->SetBrushFromMaterial(CamMtl);
}

void UCommenderScreenWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (CamImage->GetDynamicMaterial())
		UE_LOG(LogTemp, Warning, TEXT("%s"), *CamImage->GetDynamicMaterial()->GetName());
}
