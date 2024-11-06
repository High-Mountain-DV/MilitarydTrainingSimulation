// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VRWidgetActor.h"
#include "CSW/HttpActor.h"
#include "HttpLoginActor.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API AHttpLoginActor : public AVRWidgetActor
{
	GENERATED_BODY()

public:
	AHttpLoginActor();
	
	void BeginPlay() override;
	
	void RequestLogin(const FString& id, const FString& passward);
	void RequestRegister(const FString& id, const FString& nickname, const FString& passward);

private:
	FString RegisterPath = "/api/auth/signup";
	FString RegisterMethod = "POST";
	
	FString LoginPath = "/api/auth/login";
	FString LoginMethod = "POST";

};
