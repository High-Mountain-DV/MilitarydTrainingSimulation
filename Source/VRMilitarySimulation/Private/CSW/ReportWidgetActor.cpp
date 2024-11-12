// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ReportWidgetActor.h"

#include "Components/WidgetComponent.h"
#include "CSW/CSWGameInstance.h"
#include "CSW/ReportWidget.h"
#include "Interfaces/IHttpResponse.h"

void AReportWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	RequestReport();
	RequestAnalize();
}

void AReportWidgetActor::RequestReport()
{
	TMap<FString, FString> body;
	TMap<FString, FString> header;

	auto gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	auto widget = Cast<UReportWidget>(WidgetComp->GetWidget());

	// header
	header.Add("Content-Type","application/json");
	header.Add(header.Add("Authorization", gi->GetUserToken()));

	RequestToBackend(
		ReportPath,
		ReportMethod,
		header,
		"",
		[widget](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
		{
			if (bWasSuccessful && response.IsValid() && IsValid(widget))
			{
				UE_LOG(LogTemp, Log, TEXT("Request succeeded: %s"), *response->GetContentAsString());
				
				TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(*response->GetContentAsString());
				TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
			
				if (FJsonSerializer::Deserialize(reader, result))
				{
					FReportData data;

					data.nickname = result->GetStringField(TEXT("nickname"));
					data.playTime = result->GetNumberField(TEXT("playTime"));
					data.accuracy = result->GetNumberField(TEXT("accuracy"));
					data.kill = result->GetIntegerField(TEXT("kills"));
					data.injuredPlayer = result->GetIntegerField(TEXT("allyInjuries"));
					data.deadPlayer = result->GetIntegerField(TEXT("allyDeaths"));

					widget->SetReportData(data);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Request failed"));
			}
		});
}

void AReportWidgetActor::RequestAnalize()
{
	TMap<FString, FString> body;
	TMap<FString, FString> header;

	auto gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	auto widget = Cast<UReportWidget>(WidgetComp->GetWidget());

	// header
	header.Add("Content-Type","application/json");

	// make body
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());

	jsonObject->SetNumberField("id", gi->GetUserId());
	jsonObject->SetNumberField("damageDealt", 0);
	jsonObject->SetNumberField("assists", 0);
	jsonObject->SetNumberField("playTime", 0);
	jsonObject->SetNumberField("accuracy", 0);
	jsonObject->SetNumberField("score", 0);
	jsonObject->SetNumberField("awareness", true);
	jsonObject->SetNumberField("allyInjuries", 0);
	jsonObject->SetNumberField("allyDeaths", 0);
	jsonObject->SetNumberField("kills", 0);

	FString json;
	TSharedRef<TJsonWriter<TCHAR>> writer = TJsonWriterFactory<TCHAR>::Create(&json);
	FJsonSerializer::Serialize(jsonObject.ToSharedRef(), writer);

	RequestToAIServer(
		AnalizePath,
		AnalizeMethod,
		header,
		json,
		[widget](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
		{
			if (bWasSuccessful && response.IsValid() && IsValid(widget))
			{
				UE_LOG(LogTemp, Log, TEXT("Request succeeded: %s"), *response->GetContentAsString());
				
				TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(*response->GetContentAsString());
				TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
			
				if (FJsonSerializer::Deserialize(reader, result))
				{
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Request failed"));
				
			}
		});
}