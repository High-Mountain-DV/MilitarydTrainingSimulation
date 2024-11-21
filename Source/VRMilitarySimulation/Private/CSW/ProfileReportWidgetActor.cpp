// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ProfileReportWidgetActor.h"

#include "JsonObjectConverter.h"
#include "Components/WidgetComponent.h"
#include "CSW/CSWGameInstance.h"
#include "CSW/ProfileReport.h"
#include "Interfaces/IHttpResponse.h"

void AProfileReportWidgetActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProfileReportWidgetActor::RequestGraph()
{
	TMap<FString, FString> header;

	auto gi = Cast<UCSWGameInstance>(GetGameInstance());
	
	// header
	header.Add("Content-Type","application/json");
	header.Add(header.Add("Authorization", gi->GetUserToken()));

	RequestToBackend(
		ProfileReportPath,
		ProfileReportPath,
		header,
		"",
		[this](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
		{
			if (bWasSuccessful && response.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("Request succeeded: %s"), *response->GetContentAsString());
				
				TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(*response->GetContentAsString());
				TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
			
				if (FJsonSerializer::Deserialize(reader, result))
				{
					auto widget = Cast<UProfileReport>(WidgetComp->GetWidget());
					if (widget)
					{
						TArray<FProfileReportData> datas;
						FJsonObjectConverter::JsonObjectStringToUStruct(*response->GetContentAsString(), &datas);
						widget->SetProfileReportData(datas);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Request failed"));
			}
		});
}
