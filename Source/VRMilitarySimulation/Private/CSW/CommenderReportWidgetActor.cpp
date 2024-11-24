// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/CommenderReportWidgetActor.h"

#include "ImageUtils.h"
#include "Components/WidgetComponent.h"
#include "CSW/CSWGameInstance.h"
#include "CSW/ReportSlot.h"
#include "CSW/ReportWidget.h"
#include "CSW/ReportWrapperWidget.h"
#include "Interfaces/IHttpResponse.h"

void ACommenderReportWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	auto* gi = Cast<UCSWGameInstance>(GetGameInstance());
	auto* reportWrapper = Cast<UReportWrapperWidget>(WidgetComp->GetWidget());
	
	if (gi->IsCommender())
	{
		auto& arr = gi->GetTraineesId();
		for (int i = 0; i < arr.Num(); i++)
		{
			auto* report = CreateWidget<UReportSlot>(GetWorld(), ReportSlotFactory);
			reportWrapper->AddReportSlot(report);
			RequestReport(arr[i], gi->GetUserToken(), report);
		}	
	}
	else
	{
		Destroy();
	}
}

void ACommenderReportWidgetActor::RequestReport(int32 Id, const FString& Token, class UReportSlot* Report)
{
	TMap<FString, FString> header;

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
					Report->SetReportData(data.nickname, data.kill, data.accuracy, data.feedback);
					auto* reportWrapper = Cast<UReportWrapperWidget>(WidgetComp->GetWidget());
					if (reportWrapper)
						reportWrapper->SetReportData(data.playTime, data.injuredPlayer, data.deadPlayer);	
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Request failed"));
			}
		});
}
