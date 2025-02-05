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
	
	if (gi->IsCommender())
	{
		Destroy();
	}
	else
	{
		RequestReport();
	}
}

void AReportWidgetActor::RequestReport()
{
	TMap<FString, FString> header;
	auto* gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	auto* Report = Cast<UReportWidget>(WidgetComp->GetWidget());
	
	// header
	header.Add("Content-Type","application/json");
	header.Add(header.Add("Authorization", gi->GetUserToken()));
	
	RequestToBackend(
		ReportPath + FString::FromInt(gi->GetUserId()) + "/latest",
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