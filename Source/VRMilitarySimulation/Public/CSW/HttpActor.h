// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HttpFwd.h"
#include "GameFramework/Actor.h"
#include "HttpActor.generated.h"

UCLASS()
class VRMILITARYSIMULATION_API AHttpActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHttpActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
protected:
	void Request(const FString& path, const FString& method, const FString& json, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> callback);

	FString ServerUrl = "http://125.132.216.190:8091";
};
