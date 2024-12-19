// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/HttpLoginActor.h"

#include "Components/WidgetComponent.h"
#include "CSW/CSWGameInstance.h"
#include "CSW/JsonParseLib.h"
#include "CSW/LoginWidget.h"
#include "Interfaces/IHttpResponse.h"


AHttpLoginActor::AHttpLoginActor()
{
}

void AHttpLoginActor::BeginPlay()
{
	Super::BeginPlay();

	auto* gi = Cast<UCSWGameInstance>(GetGameInstance());
	if (gi)
	{
		if (gi->GetNickname().IsEmpty())
		{
			auto* widget = Cast<ULoginWidget>(WidgetComp->GetWidget());
			if (widget)
				widget->SetHttpLoginActor(this);
		}
		else
		{
			WidgetComp->SetWidget(CreateWidget<UUserWidget>(GetWorld(), PreLobbyWidgetFactory));
			Open();
		}
	}
	
}

void AHttpLoginActor::RequestLogin(const FString& id, const FString& password)
{
	TMap<FString, FString> body;
	TMap<FString, FString> header;

	body.Add("loginId", id);
	body.Add("password", password);

	header.Add("Content-Type","application/json");
	auto* gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	if (gi == nullptr)
		return ;
	RequestToBackend(LoginPath, LoginMethod, header, UJsonParseLib::MakeJson(body), [gi, this](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
	{
		if (bWasSuccessful && response.IsValid() && IsValid(gi))
		{
			UE_LOG(LogTemp, Log, TEXT("Request succeeded: %s"), *response->GetContentAsString());

			TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(*response->GetContentAsString());
			TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
			TMap<FString, FString> returnValue;
		
			if (FJsonSerializer::Deserialize(reader, result))
			{
				if (result->GetBoolField(TEXT("success")))
				{
					TSharedPtr<FJsonObject> data = result->GetObjectField(TEXT("response"));
					gi->SetUserId(data->GetIntegerField(TEXT("id")));
					gi->SetUserNickname(data->GetStringField(TEXT("nickname")));
				}
			}
			gi->SetUserToken(response->GetHeader("Authorization"));
			WidgetComp->SetWidget(CreateWidget<UUserWidget>(GetWorld(), PreLobbyWidgetFactory));
			bLogin = true;
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
	RequestToBackend(RegisterPath, RegisterMethod, header, UJsonParseLib::MakeJson(Body), [](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
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
