// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSWGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "CustomizingButton.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UCustomizingButton : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	class UButton* Button;

	UFUNCTION()
	void OnClickButton();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Texture;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Path;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<ECustomizingCategory> Category;
	
};
