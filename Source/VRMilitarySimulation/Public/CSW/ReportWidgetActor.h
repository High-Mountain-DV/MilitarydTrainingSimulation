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
	void RequestReport(int32 Id, const FString& Token, class UReportWidget* Report);
	// void RequestAnalize();

private:
	FString ReportPath = "/api/combats/user/latest";
	FString ReportMethod = "GET";
	
	FString AnalizePath = "/training/analyze";
	FString AnalizeMethod = "POST";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TSubclassOf<UReportWidget> ReportWidgetFactory;
	
};
