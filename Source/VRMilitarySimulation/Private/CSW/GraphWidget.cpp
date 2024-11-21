// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/GraphWidget.h"

int32 UGraphWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{

	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(),
		Points,
		ESlateDrawEffect::None,
		GraphColor,
		true,
		2.0f
	);
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
	                          bParentEnabled);
}

void UGraphWidget::SetPoints(int32 Order, float Value)
{
	Points.Add(FVector2D(Order * 100, Value));
}

void UGraphWidget::SetGraphColor(const FLinearColor& color)
{
	GraphColor = color;
}
