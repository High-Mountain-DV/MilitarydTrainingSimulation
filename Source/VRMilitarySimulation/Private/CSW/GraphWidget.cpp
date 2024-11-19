// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/GraphWidget.h"

int32 UGraphWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{

	TArray<FVector2D> Points;
	Points.Add(FVector2D(200, 300));
	Points.Add(FVector2D(400, 100));
	Points.Add(FVector2D(600, 450));
	Points.Add(FVector2D(800, 210));
	Points.Add(FVector2D(1000, 70));
	
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(),
		Points,
		ESlateDrawEffect::None,
		FLinearColor::Green,
		true,
		2.0f
	);
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                          bParentEnabled);
}
