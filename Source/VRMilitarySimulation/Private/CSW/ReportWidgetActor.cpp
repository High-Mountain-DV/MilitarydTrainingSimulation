// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/ReportWidgetActor.h"

#include "ImageUtils.h"
#include "Components/WidgetComponent.h"
#include "CSW/CSWGameInstance.h"
#include "CSW/ReportWidget.h"
#include "CSW/ReportWrapperWidget.h"
#include "Interfaces/IHttpResponse.h"

void AReportWidgetActor::BeginPlay()
{
	Super::BeginPlay();


	auto* gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	auto* reportWrapper = Cast<UReportWrapperWidget>(WidgetComp->GetWidget());
	
	if (gi->IsCommender())
	{
		auto& arr = gi->GetTraineesId();
		for (int i = 0; i < arr.Num(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("trainee ID: %d"), arr[i]);
			auto* report = CreateWidget<UReportWidget>(GetWorld(), ReportWidgetFactory);
			reportWrapper->AppendReport(report);
			RequestReport(arr[i], gi->GetUserToken(), report);
		}
	}
	else
	{
		auto* report = CreateWidget<UReportWidget>(GetWorld(), ReportWidgetFactory);
		reportWrapper->AppendReport(report);
		RequestReport(gi->GetUserId(), gi->GetUserToken(), report);
	}
}

void AReportWidgetActor::RequestReport(int32 Id, const FString& Token, UReportWidget* Report)
{
	TMap<FString, FString> header;

	UE_LOG(LogTemp, Warning, TEXT("%d"), Id);
	// header
	header.Add("Content-Type","application/json");
	header.Add(header.Add("Authorization", Token));

	RequestToBackend(
		ReportPath + FString::FromInt(Id) + "/latest",
		ReportMethod,
		header,
		"",
		[Report, this](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
		{
			if (bWasSuccessful && response.IsValid() && IsValid(Report))
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
					data.imageUrl = result->GetStringField(TEXT("radarChart"));
					data.feedback = result->GetStringField(TEXT("feedback"));

					RequestToS3Image(data.imageUrl, [Report](const FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
					{
						if (bWasSuccessful && response.IsValid() && IsValid(Report))
						{
							TArray<uint8> data = response->GetContent();
							FString imagePath = FPaths::ProjectPersistentDownloadDir()+"/RadarGraph.png";
							FFileHelper::SaveArrayToFile(data, *imagePath);
							UTexture2D* texture = FImageUtils::ImportBufferAsTexture2D(data);
							Report->SetRadarGraph(texture);
						}
					});
					Report->SetReportData(data);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Request failed"));
			}
		});
}

// void AReportWidgetActor::RequestAnalize()
// {
// 	TMap<FString, FString> body;
// 	TMap<FString, FString> header;
//
// 	auto gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
// 	auto widget = Cast<UReportWidget>(WidgetComp->GetWidget());
//
// 	// header
// 	header.Add("Content-Type","application/json");
//
// 	// make body
// 	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());
//
// 	jsonObject->SetNumberField("id", gi->GetUserId());
// 	jsonObject->SetNumberField("damageDealt", 0);
// 	jsonObject->SetNumberField("assists", 0);
// 	jsonObject->SetNumberField("playTime", 0);
// 	jsonObject->SetNumberField("accuracy", 0);
// 	jsonObject->SetNumberField("score", 0);
// 	jsonObject->SetNumberField("awareness", true);
// 	jsonObject->SetNumberField("allyInjuries", 0);
// 	jsonObject->SetNumberField("allyDeaths", 0);
// 	jsonObject->SetNumberField("kills", 0);
//
// 	FString json;
// 	TSharedRef<TJsonWriter<TCHAR>> writer = TJsonWriterFactory<TCHAR>::Create(&json);
// 	FJsonSerializer::Serialize(jsonObject.ToSharedRef(), writer);
//
// 	RequestToAIServer(
// 		AnalizePath,
// 		AnalizeMethod,
// 		header,
// 		json,
// 		[widget](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
// 		{
// 			if (bWasSuccessful && response.IsValid() && IsValid(widget))
// 			{
// 				UE_LOG(LogTemp, Log, TEXT("Request succeeded: %s"), *response->GetContentAsString());
// 				
// 				TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(*response->GetContentAsString());
// 				TSharedPtr<FJsonObject> result = MakeShareable(new FJsonObject());
// 			
// 				if (FJsonSerializer::Deserialize(reader, result))
// 				{
// 				}
// 			}
// 			else
// 			{
// 				UE_LOG(LogTemp, Warning, TEXT("Request failed"));
// 				
// 			}
// 		});
// }