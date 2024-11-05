// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KeyboardWidgetActor.h"
#include "Blueprint/UserWidget.h"
#include "KeyboardWidget.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FToKoreanDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FToUpperDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FToLowerDelegate);



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

	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonEnter;

	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonBackSpace;

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonSwitchLanguage;

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonLeftShift;

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonRightShift;

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonCapsLock;
	
	
	UFUNCTION()
	void OnClickClose();
	
	UFUNCTION()
	void OnClickEnter();

	UFUNCTION()
	void OnClickBackSpace();

	UFUNCTION()
	void OnClickSwitchLanguage();

	UFUNCTION()
	void OnClickShift();

	UPROPERTY()
	FToKoreanDelegate ToKoreanDelegate;

	UPROPERTY()
	FToUpperDelegate ToUpperDelegate;

	UPROPERTY()
	FToLowerDelegate ToLowerDelegate;

	bool IsEnglish();
	bool IsUpper();
private:
	UPROPERTY()
	AKeyboardWidgetActor* KeyboardActor;

	bool bUpper = false;
	bool bEnglish = true;
};
