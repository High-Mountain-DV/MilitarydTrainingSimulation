// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/HttpLoginActor.h"

#include "Components/WidgetComponent.h"
#include "CSW/JsonParseLib.h"
#include "CSW/LoginWidget.h"
#include "Interfaces/IHttpResponse.h"

AHttpLoginActor::AHttpLoginActor()
{
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	SetRootComponent(SceneComp);
	
	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("RootComp"));
	WidgetComp->SetupAttachment(RootComponent);
}

void AHttpLoginActor::BeginPlay()
{
	Super::BeginPlay();

	auto* widget = Cast<ULoginWidget>(WidgetComp->GetWidget());
	if (widget)
		widget->SetHttpLoginActor(this);
}

void AHttpLoginActor::RequestLogin(const FString& id, const FString& passward)
{
	TMap<FString, FString> userData;

	userData.Add("loginId", id);
	userData.Add("passward", passward);

	Request(LoginPath, LoginMethod, UJsonParseLib::MakeJson(userData), [](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
	{
		if (bWasSuccessful && response.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("Request succeeded: %s"), *response->GetContentAsString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Request failed"));
		}
	});
}

void AHttpLoginActor::RequestRegister(const FString& id, const FString& nickname, const FString& passward)
{
	TMap<FString, FString> userData;

	userData.Add("loginId", id);
	userData.Add("passward", passward);
	userData.Add("confirmPassward", passward);
	userData.Add("nickname", nickname);
	
	Request(RegisterPath, RegisterMethod, UJsonParseLib::MakeJson(userData), [](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
	{
		if (bWasSuccessful && response.IsValid())
		{
			UE_LOG(LogTemp, Log, TEXT("Request succeeded: %s"), *response->GetContentAsString());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Request failed"));
		}
	});
}
