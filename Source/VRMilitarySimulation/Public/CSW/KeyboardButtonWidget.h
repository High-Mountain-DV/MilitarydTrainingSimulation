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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString EnValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString KoValue;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Button;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Value;
	
	UFUNCTION()
	void OnClickButton();

	UFUNCTION()
	void ToKorean();

	UFUNCTION()
	void ToUpper();

	UFUNCTION()
	void ToLower();

private:
	UPROPERTY()
	class UKeyboardWidget* KeyboardWidget;
};
