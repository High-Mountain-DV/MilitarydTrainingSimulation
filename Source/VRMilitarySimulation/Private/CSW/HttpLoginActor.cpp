// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/HttpLoginActor.h"

#include "Components/WidgetComponent.h"
#include "CSW/CSWGameInstance.h"
#include "CSW/JsonParseLib.h"
#include "CSW/LoginWidget.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"

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

void AHttpLoginActor::RequestLogin(const FString& id, const FString& password)
{
	TMap<FString, FString> body;
	TMap<FString, FString> header;

	body.Add("loginId", id);
	body.Add("password", password);

	header.Add("Content-Type","application/json");
	auto* gi = Cast<UCSWGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	Request(LoginPath, LoginMethod, header, UJsonParseLib::MakeJson(body), [gi](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
	{
		if (bWasSuccessful && response.IsValid() && IsValid(gi))
		{
			UE_LOG(LogTemp, Log, TEXT("Request succeeded: %s"), *response->GetContentAsString());
			gi->SetUserToken(response->GetHeader("Authorization"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Request failed"));
		}
	});
}

void AHttpLoginActor::RequestRegister(const FString& id, const FString& nickname, const FString& passward)
{
	TMap<FString, FString> Body;
	TMap<FString, FString> header;


	Body.Add("loginId", id);
	Body.Add("password", passward);
	Body.Add("confirmPassword", passward);
	Body.Add("nickname", nickname);

	header.Add("Content-Type","application/json");
	Request(RegisterPath, RegisterMethod, header, UJsonParseLib::MakeJson(Body), [](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
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
