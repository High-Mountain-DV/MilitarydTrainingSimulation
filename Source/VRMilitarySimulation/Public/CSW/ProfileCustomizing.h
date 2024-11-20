// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProfileCustomizing.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UProfileCustomizing : public UUserWidget
{
	GENERATED_BODY()

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
private:
	bool bDrag = false;
	FVector2D DragStartPos;
};
