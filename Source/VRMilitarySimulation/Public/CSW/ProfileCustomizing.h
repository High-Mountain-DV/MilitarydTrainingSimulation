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

	
public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBoxMark;
	
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBoxCream;
	
	UPROPERTY(meta=(BindWidget))
	class UCheckBox* CheckBoxAcc;
	
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	UFUNCTION()
	void OnCheckMark(bool bIsChecked);

	UFUNCTION()
	void OnCheckCream(bool bIsChecked);

	UFUNCTION()
	void OnCheckAcc(bool bIsChecked);

private:
	const int MARK_IDX = 0;
	const int CREAM_IDX = 1;
	const int ACC_IDX = 2;
};
