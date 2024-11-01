// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	// Login =================================================
	UPROPERTY(meta=(BindWidget))
	class UEditableText* Lg_Input_Id;

	UPROPERTY(meta=(BindWidget))
	class UEditableText* Lg_Input_Passward;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Lg_Button_Login;

	UPROPERTY(meta=(BindWidget))
	class UButton* Lg_Button_GoRegister;

	UFUNCTION()
	void Lg_OnClickLogin();

	UFUNCTION()
	void Lg_OnClickGoRegister();

	
	
	// Register =============================================
	UPROPERTY(meta=(BindWidget))
	class UEditableText* Rg_Input_Id;

	UPROPERTY(meta=(BindWidget))
	class UEditableText* Rg_Input_Nickname;

	UPROPERTY(meta=(BindWidget))
	class UEditableText* Rg_Input_Passward;

	UPROPERTY(meta=(BindWidget))
	class UButton* Rg_Button_Register;

	UPROPERTY(meta=(BindWidget))
	class UButton* Rg_Button_Cancel;

	UFUNCTION()
	void Rg_OnClickRegister();

	UFUNCTION()
	void Rg_OnClickCancel();
	
	void SetHttpLoginActor(class AHttpLoginActor* actor);
	
private:
	UPROPERTY()
	AHttpLoginActor* HttpActor;
};
