// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReportWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UReportWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonGoLobby;

	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonReplay;

	UFUNCTION()
	void OnClickGoLobby();

	UFUNCTION()
	void OnClickReplay();
};
