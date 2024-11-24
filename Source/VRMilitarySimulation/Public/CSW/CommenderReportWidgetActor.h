// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSW/VRWidgetActor.h"
#include "CommenderReportWidgetActor.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API ACommenderReportWidgetActor : public AVRWidgetActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	
	void RequestReport(int32 Id, const FString& Token, class UReportSlot* Report);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TSubclassOf<UUserWidget> ReportSlotFactory;

	FString ReportPath = "/api/combats/commander/user/";
	FString ReportMethod = "GET";
};
