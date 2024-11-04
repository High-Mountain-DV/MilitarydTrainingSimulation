// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSW/VRWidgetActor.h"
#include "KeyboardWidgetActor.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API AKeyboardWidgetActor : public AVRWidgetActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	void SetScreenWidget(class UVRWidget* widget);

	void PressKey(const FString& key);

private:
	UPROPERTY()
	UVRWidget* ScreenWidget;
};
