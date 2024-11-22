// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DraggableScrollBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UDraggableScrollBoxWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta=(BindWidget))
	class UScrollBox* ScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CenterAxisOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ScaleSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCenterStart = false;
	
private:
	void UpdateScrollOffset(const FPointerEvent& InMouseEvent);

	void UpdateScrollChildrenScale();
	
	bool bDrag = false;
	bool bXScroll = false;
	FVector2D DragStartPos;
};
