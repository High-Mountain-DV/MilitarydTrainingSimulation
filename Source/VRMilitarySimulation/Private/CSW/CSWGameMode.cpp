// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/CSWGameMode.h"

#include "CSW/CommenderScreen.h"
#include "Kismet/GameplayStatics.h"

void ACSWGameMode::BeginPlay()
{
	Super::BeginPlay();

}

void ACSWGameMode::CompleteOnePlayerLoading(UMaterialInstanceDynamic* CamMtl)
{
	auto* CommenderScreen = Cast<ACommenderScreen>(UGameplayStatics::GetActorOfClass(GetWorld(), ACommenderScreen::StaticClass()));
	if (CommenderScreen)
		CommenderScreen->AddPlayerScreen(CamMtl);	
}
