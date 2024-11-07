// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/CSWGameMode.h"

#include "CSW/CommenderScreen.h"
#include "CSW/CSWGameInstance.h"
#include "Kismet/GameplayStatics.h"

void ACSWGameMode::BeginPlay()
{
	Super::BeginPlay();

	auto *gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
		gi->StartRecording();
}

void ACSWGameMode::CompleteOnePlayerLoading(UMaterialInstanceDynamic* CamMtl)
{
	auto* CommenderScreen = Cast<ACommenderScreen>(UGameplayStatics::GetActorOfClass(GetWorld(), ACommenderScreen::StaticClass()));
	if (CommenderScreen)
		CommenderScreen->AddPlayerScreen(CamMtl);	
}

void ACSWGameMode::EndGame()
{
	auto *gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());

	if (gi)
	{
		gi->StopRecording();
		gi->ExitSession();
		gi->GoReportRoom();
	}
}
