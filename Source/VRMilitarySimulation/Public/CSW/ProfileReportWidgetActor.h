// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSW/VRWidgetActor.h"
#include "ProfileReportWidgetActor.generated.h"

/**
 * 
 */

UCLASS()
class VRMILITARYSIMULATION_API AProfileReportWidgetActor : public AVRWidgetActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void RequestGraph();

private:
	FString ProfileReportPath;
	FString ProfileReportMethod;
};
