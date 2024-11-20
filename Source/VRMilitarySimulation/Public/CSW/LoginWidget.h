// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRWidget.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API ULoginWidget : public UVRWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	// Login =================================================
	UPROPERTY(meta=(BindWidget))
	class UButton* Lg_Button_InputId;

	UPROPERTY(meta=(BindWidget))
	class UButton* Lg_Button_InputPassward;
	
	UPROPERTY(meta=(BindWidget))
	class UEditableText* Lg_Text_Id;

	UPROPERTY(meta=(BindWidget))
	class UEditableText* Lg_Text_Passward;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* Lg_Button_Login;

	UPROPERTY(meta=(BindWidget))
	class UButton* Lg_Button_GoRegister;

	UFUNCTION()
	void Lg_OnClickLogin();

	UFUNCTION()
	void Lg_OnClickGoRegister();

	UFUNCTION()
	void Lg_OnClickInputId();

	UFUNCTION()
	void Lg_OnClickInputPassward();

	
	
	// Register =============================================
	UPROPERTY(meta=(BindWidget))
	class UButton* Rg_Button_InputId;

	UPROPERTY(meta=(BindWidget))
	class UButton* Rg_Button_InputNickname;

	UPROPERTY(meta=(BindWidget))
	class UButton* Rg_Button_InputPassward;

	UPROPERTY(meta=(BindWidget))
	class UButton* Rg_Button_Register;

	UPROPERTY(meta=(BindWidget))
	class UButton* Rg_Button_Cancel;

	UPROPERTY(meta=(BindWidget))
	UEditableText* Rg_Text_Id;

	UPROPERTY(meta=(BindWidget))
	UEditableText* Rg_Text_Nickname;

	UPROPERTY(meta=(BindWidget))
	UEditableText* Rg_Text_Passward;


	UFUNCTION()
	void Rg_OnClickRegister();

	UFUNCTION()
	void Rg_OnClickCancel();

	UFUNCTION()
	void Rg_OnClickInputId();

	UFUNCTION()
	void Rg_OnClickInputNickname();

	UFUNCTION()
	void Rg_OnClickInputPassward();
	
	void SetHttpLoginActor(class AHttpLoginActor* actor);
	
private:
	UPROPERTY()
	AHttpLoginActor* HttpActor;
};
