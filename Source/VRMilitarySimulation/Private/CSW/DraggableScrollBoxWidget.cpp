// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/DraggableScrollBoxWidget.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"

void UDraggableScrollBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bXScroll = ScrollBox->GetOrientation() == Orient_Horizontal;

	if (bCenterStart)
		ScrollBox->SetScrollOffset(CenterAxisOffset);

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, [this]()
	{
		UpdateScrollChildrenScale();
	}, 0.5f,false);
}

FReply UDraggableScrollBoxWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		bDrag = true;
		DragStartPos = InMouseEvent.GetScreenSpacePosition();

		return FReply::Handled().CaptureMouse(TakeWidget());
	}

	return FReply::Unhandled();
}

FReply UDraggableScrollBoxWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bDrag)
	{
		UpdateScrollOffset(InMouseEvent);
		UpdateScrollChildrenScale();
	}
	
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UDraggableScrollBoxWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bDrag && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		bDrag = false;

		return FReply::Handled().ReleaseMouseCapture();
	}
	
	return FReply::Unhandled();
}

void UDraggableScrollBoxWidget::UpdateScrollOffset(const FPointerEvent& InMouseEvent)
{
	// 스크롤 위치를 업데이트
	FVector2D CurrentPos = InMouseEvent.GetScreenSpacePosition(); //마우스 위치
	float CurrentOffset = ScrollBox->GetScrollOffset(); //스크롤 위치
	float Delta = bXScroll ? CurrentPos.X - DragStartPos.X : CurrentPos.Y - DragStartPos.Y; //마우스와 스크롤 위치변화
	float Sensitivity = 3;	// 스크롤 민감도

	ScrollBox->SetScrollOffset(CurrentOffset - Delta * 3);
	DragStartPos = CurrentPos;
}

void UDraggableScrollBoxWidget::UpdateScrollChildrenScale()
{
	// update scale
	FGeometry ScrollBoxGeometry = ScrollBox->GetCachedGeometry();
	float MaxOffset = bXScroll ? ScrollBox->GetDesiredSize().X : ScrollBox->GetDesiredSize().Y;

	for (UWidget* child : ScrollBox->GetAllChildren())
	{
		FGeometry childGeometry = child->GetCachedGeometry();
		float elementCenter = bXScroll ? childGeometry.GetAbsolutePosition().X : childGeometry.GetAbsolutePosition().Y;
		float distanceFromCenter = FMath::Abs(elementCenter - CenterAxisOffset);
		float scale = 1.f - (distanceFromCenter / CenterAxisOffset) * ScaleSensitivity;
		
		child->SetRenderScale(FVector2D(scale));
		child->SetRenderOpacity(scale);
	}
}
