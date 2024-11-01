// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/HttpActor.h"

#include "HttpModule.h"

// Sets default values
AHttpActor::AHttpActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHttpActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHttpActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHttpActor::Request(const FString& path, const FString& method, const FString& json, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> callback)
{
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();
	FString url = ServerUrl + path;
	
	req->SetURL(url);
	req->SetVerb(method);
	req->SetHeader(TEXT("content-type"), TEXT("application/json"));
	req->SetContentAsString(json);
	req->OnProcessRequestComplete().BindLambda(callback);
	req->ProcessRequest();
}

