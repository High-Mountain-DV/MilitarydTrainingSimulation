// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/CommenderScreenWidget.h"

#include "Components/Image.h"

void UCommenderScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCommenderScreenWidget::AddPlayerScreen(UMaterialInstanceDynamic* CamMtl)
{
	if (Idx < 4)
	{
		if (Cams.IsEmpty())
		{
			MakeCamArr();
		}
		Cams[Idx++]->SetBrushFromMaterial(CamMtl);
	}
}

void UCommenderScreenWidget::MakeCamArr()
{
	Cams.Add(CamImage0);
	Cams.Add(CamImage1);
	Cams.Add(CamImage2);
	Cams.Add(CamImage3);
}

void UCommenderScreenWidget::SelectScreen(int32 idx)
{
	if (!Cams.IsEmpty())
	{
		WholeScreen->SetBrush(Cams[idx]->GetBrush());
		SelectedIdx = idx;
		WholeScreen->SetVisibility(ESlateVisibility::Visible);
	}
}

void UCommenderScreenWidget::SelectNextScreen()
{
	if (WholeScreen->GetVisibility() == ESlateVisibility::Collapsed)
		return ;
	
	if (!Cams.IsEmpty())
	{
		WholeScreen->SetBrush(Cams[++SelectedIdx %= Cams.Num()]->GetBrush());
	}
}

void UCommenderScreenWidget::SelectPrevScreen()
{
	if (WholeScreen->GetVisibility() == ESlateVisibility::Collapsed)
		return ;

	if (!Cams.IsEmpty())
	{
		if (--SelectedIdx < 0)
			SelectedIdx = Cams.Num() - 1;
		WholeScreen->SetBrush(Cams[SelectedIdx]->GetBrush());
	}
}

void UCommenderScreenWidget::UnselectScreen()
{
	WholeScreen->SetVisibility(ESlateVisibility::Collapsed);
}
