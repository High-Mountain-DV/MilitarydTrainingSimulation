// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommenderScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UCommenderScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	void AddPlayerScreen(UMaterialInstanceDynamic* CamMtl);

	void SelectScreen(int32 idx);
	void UnselectScreen();

private:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess))
	class UImage* CamImage0;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess))
	class UImage* CamImage1;
	
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess))
	class UImage* CamImage2;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess))
	class UImage* CamImage3;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess))
	class UImage* WholeScreen;

	TArray<UImage*> Cams;
	int32 Idx = 0;
	int32 SelectedIdx = 0;

};
