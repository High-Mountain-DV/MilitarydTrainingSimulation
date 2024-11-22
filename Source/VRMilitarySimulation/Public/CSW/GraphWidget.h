// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GraphWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UGraphWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	void SetPoints(int32 Order, float Value);
	void SetGraphColor(const FLinearColor& color);
	
private:
	TArray<FVector2D> Points;
	FLinearColor GraphColor;
};
