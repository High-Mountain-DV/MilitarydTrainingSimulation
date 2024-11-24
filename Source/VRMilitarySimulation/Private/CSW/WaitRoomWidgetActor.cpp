// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/WaitRoomWidgetActor.h"

#include "ImaginaryBlueprintData.h"
#include "Components/WidgetComponent.h"
#include "CSW/CSWGameInstance.h"
#include "CSW/WaitRoomWidget.h"
#include "Kismet/GameplayStatics.h"

void AWaitRoomWidgetActor::BeginPlay()
{
	Super::BeginPlay();

	auto* gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		auto *SessionInfo = gi->GetSessionInfo();
		if (SessionInfo)
			MaxPlayerCnt = SessionInfo->SessionSettings.NumPublicConnections;

	}
}

void AWaitRoomWidgetActor::AddPlayerPanel(const FString& nickname)
{
	UWaitRoomWidget* widget = Cast<UWaitRoomWidget>(WidgetComp->GetWidget());

	if (widget)
	{
		widget->AddPlayerPanel(nickname);
		if (++PlayerCnt == MaxPlayerCnt)
		{
			TArray<AActor *> actors;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("Wall")), actors);
			
			if (!actors.IsEmpty())
			{
				GetWorld()->SpawnActor<AActor>(BuildingModel, SpawnSpot)->SetActorScale3D(SpawnSpot.GetScale3D());
				actors[0]->Destroy();
				WidgetComp->SetVisibility(false);
			}
		}
	}
}
