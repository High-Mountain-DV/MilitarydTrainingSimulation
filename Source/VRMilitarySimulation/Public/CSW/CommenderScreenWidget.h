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
	void AddPlayerScreen(UMaterialInstanceDynamic* CamMtl);

	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
private:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess))
	class UImage* CamImage;


};
