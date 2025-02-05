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
	
	void RequestToBackend(const FString& path, const FString& method, const TMap<FString, FString>& header, const FString& body, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> callback);
	void RequestToAIServer(const FString& path, const FString& method, const TMap<FString, FString>& header, const FString& body, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> callback);
	void RequestToS3Image(const FString& url, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> callback);
	
protected:
	FString BackendUrl = "http://125.132.216.190:8091";
	FString AIServerUrl = "https://husky-fun-ray.ngrok-free.app";
};
