// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ProfileCustomizing.h"

FReply UProfileCustomizing::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		bDrag = true;
		DragStartPos = InMouseEvent.GetScreenSpacePosition();

		return FReply::Handled().CaptureMouse(TakeWidget());
	}
	return FReply::Unhandled();
}

FReply UProfileCustomizing::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bDrag)
	{
		FVector2D CurrentPos = InMouseEvent.GetScreenSpacePosition();

		if (DragStartPos.X < CurrentPos.X)
		{
			UE_LOG(LogTemp, Warning, TEXT("LEFT!!!"));
		}
		else if (DragStartPos.X > CurrentPos.X)
		{
			UE_LOG(LogTemp, Warning, TEXT("Right!!!"));
		}
	}
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UProfileCustomizing::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bDrag && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bDrag = false;

		return FReply::Handled().ReleaseMouseCapture();
	}
	
	return FReply::Unhandled();
}
