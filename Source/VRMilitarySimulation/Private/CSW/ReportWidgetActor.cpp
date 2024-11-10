// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ReportWidgetActor.h"

#include "Components/WidgetComponent.h"
#include "CSW/CSWGameInstance.h"
#include "CSW/ReportWidget.h"
#include "Interfaces/IHttpResponse.h"

void AReportWidgetActor::RequestReport()
{
	TMap<FString, FString> body;
	TMap<FString, FString> header;

	auto gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	auto widget = Cast<UReportWidget>(WidgetComp->GetWidget());

	// header
	header.Add("Content-Type","application/json");
	header.Add(header.Add("Authorization", gi->GetUserToken()));

	Request(RegisterPath + FString::FromInt(
	gi->GetUserId()),
		RegisterMethod,
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
					data.playTime = result->GetNumberField(TEXT("play_time"));
					data.accuracy = result->GetNumberField(TEXT("accuracy"));
					data.kill = result->GetIntegerField(TEXT("kills"));
					data.injuredPlayer = result->GetIntegerField(TEXT("ally_injuries"));
					data.deadPlayer = result->GetIntegerField(TEXT("ally_dead"));

					widget->SetReportData(data);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Request failed"));
			}
		});
}
