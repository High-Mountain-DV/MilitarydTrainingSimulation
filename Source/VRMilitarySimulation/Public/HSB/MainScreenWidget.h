// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UMainScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Enter;

	UPROPERTY(meta=(BindWidget))
	class UButton* Btn_Tutorial;

	UFUNCTION()
	void Enter_Main();

	UFUNCTION()
	void Enter_Tutorial();
};
