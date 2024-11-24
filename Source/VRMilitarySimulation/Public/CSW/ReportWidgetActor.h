// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSW/VRWidgetActor.h"
#include "ReportWidgetActor.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API AReportWidgetActor : public AVRWidgetActor
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	void RequestReport();
	
private:
	FString ReportPath = "/api/combats/commander/user/";
	FString ReportMethod = "GET";
	
};
