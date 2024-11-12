// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReportWrapperWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UReportWrapperWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonGoLeft;

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonGoRight;

	UFUNCTION()
	void OnClickGoLeft();

	UFUNCTION()
	void OnClickGoRight();

	void AppendReport(class UReportWidget* Report);
};
