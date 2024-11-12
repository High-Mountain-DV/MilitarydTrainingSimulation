// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API UReplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta=(BindWidget))
	class USlider* TimeSlider;

	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonPlay;

	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonRewind;
	
	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonSkip;

	UPROPERTY(meta=(BindWidget))
	UButton* ButtonExit;
	

	UFUNCTION()
	void OnClickTimeSliderBegin();

	UFUNCTION()
	void OnClickTimeSliderEnd();

	UFUNCTION()
	void OnClickPlay();

	UFUNCTION()
	void OnClickRewind();

	UFUNCTION()
	void OnClickSkip();

	UFUNCTION()
	void OnClickExit();
	
};
