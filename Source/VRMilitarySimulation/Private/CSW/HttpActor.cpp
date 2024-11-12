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

void AHttpActor::RequestToBackend(const FString& path, const FString& method, const TMap<FString, FString>& header, const FString& body, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> callback)
{
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();
	FString url = BackendUrl + path;
	
	req->SetURL(url);
	req->SetVerb(method);
	for (auto pair : header)
	{
		req->SetHeader(pair.Key, pair.Value);
	}
	req->SetContentAsString(body);
	req->OnProcessRequestComplete().BindLambda(callback);
	req->ProcessRequest();
}

void AHttpActor::RequestToAIServer(const FString& path, const FString& method, const TMap<FString, FString>& header,
	const FString& body, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> callback)
{
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();
	FString url = AIServerUrl + path;
	
	req->SetURL(url);
	req->SetVerb(method);
	req->SetTimeout(180);
	for (auto pair : header)
	{
		req->SetHeader(pair.Key, pair.Value);
	}
	req->SetContentAsString(body);
	req->OnProcessRequestComplete().BindLambda(callback);
	req->ProcessRequest();
}

void AHttpActor::RequestToS3Image(const FString& url, TFunction<void(FHttpRequestPtr, FHttpResponsePtr, bool)> callback)
{
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();

	req->SetURL(url);
	req->SetVerb(TEXT("GET"));
	req->SetHeader(TEXT("content-type"), TEXT("image/jpeg"));

	req->OnProcessRequestComplete().BindLambda(callback);
	req->ProcessRequest();
}

