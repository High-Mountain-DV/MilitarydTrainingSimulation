// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSW/VRWidgetActor.h"
#include "WaitRoomWidgetActor.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API AWaitRoomWidgetActor : public AVRWidgetActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AddPlayerPanel(const FString& nickname);
	
};
