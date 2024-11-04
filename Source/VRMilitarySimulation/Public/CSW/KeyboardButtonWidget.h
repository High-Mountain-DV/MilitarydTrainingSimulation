// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KeyboardButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UKeyboardButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Button;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Value;

	UFUNCTION()
	void OnClickButton();
};
