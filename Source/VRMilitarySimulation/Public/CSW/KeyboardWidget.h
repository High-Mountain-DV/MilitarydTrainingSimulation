// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KeyboardWidgetActor.h"
#include "Blueprint/UserWidget.h"
#include "KeyboardWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UKeyboardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	void PressKey(const FString& value);
	
	void SetKeyboardActor(AKeyboardWidgetActor* actor);

	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonClose;

	UFUNCTION()
	void OnClickClose();
	
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonEnter;

	UFUNCTION()
	void OnClickEnter();
	

private:
	UPROPERTY()
	AKeyboardWidgetActor* KeyboardActor;
};
