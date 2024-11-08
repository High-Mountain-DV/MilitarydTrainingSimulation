// Fill out your copyright notice in the Description page of Project Settings.


#include "CSW/CSWGameMode.h"

#include "CSW/CommenderScreen.h"
#include "CSW/CSWGameInstance.h"
#include "CSW/HttpActor.h"
#include "CSW/JsonParseLib.h"
#include "HSB/PlayerVRCharacter.h"
#include "Interfaces/IHttpResponse.h"
#include "Kismet/GameplayStatics.h"
#include "VRMilitarySimulation/SG_Enemy.h"

void ACSWGameMode::BeginPlay()
{
	Super::BeginPlay();

	auto *gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
		gi->StartRecording();
}

void ACSWGameMode::CompleteOnePlayerLoading(UMaterialInstanceDynamic* CamMtl, FString id)
{
	auto* CommenderScreen = Cast<ACommenderScreen>(UGameplayStatics::GetActorOfClass(GetWorld(), ACommenderScreen::StaticClass()));
	if (CommenderScreen)
		CommenderScreen->AddPlayerScreen(CamMtl);
	UserIds.Add(id);
}

void ACSWGameMode::EndGame()
{
	//CollectPlayerLog();
	//CollectEnemyLog();
	//for (id : UserIds)
	//PostCombatLog();


	//이건 나중에 델리게이트로 옮길 예정
	auto *gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());

	if (gi)
	{
		gi->StopRecording();
		gi->ExitSession();
		gi->GoReportRoom();
	}
}

void ACSWGameMode::CollectPlayerLog()
{
	TArray<AActor*> outActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerVRCharacter::StaticClass(), outActors);
	for (AActor* actor : outActors)
	{
		APlayerVRCharacter* player = Cast<APlayerVRCharacter>(actor);

		FString id = player->GetActorLabel();
		int shootingCnt = player->GetShootingCnt();
		ShootLog[id] += shootingCnt;
	}
}

void ACSWGameMode::CollectEnemyLog()
{
	TArray<AActor*> outActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASG_Enemy::StaticClass(), outActors);
	for (AActor* actor : outActors)
	{
		ASG_Enemy* enemy = Cast<ASG_Enemy>(actor);

		TMap<FString, TTuple<int32, float>> hitLog = enemy->GetHitLog();
		for (auto log : hitLog)
		{
			HitLog[log.Key].Get<0>() += log.Value.Get<0>();
			HitLog[log.Key].Get<1>() += log.Value.Get<1>();
		}
	}
}

void ACSWGameMode::PostCombatLog(const FString& id)
{
	AHttpActor actor;
	
	TMap<FString, FString> header;
	TMap<FString, FString> body;

	auto gi = Cast<UCSWGameInstance>(GetGameInstance());
	header.Add("Content-Type","application/json");
	actor.Request(
		"/api/combat",
		"POST",
		header,
		UJsonParseLib::MakeJson(body),
		[gi](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
		{
			if (bWasSuccessful && response.IsValid() && IsValid(gi))
			{
				UE_LOG(LogTemp, Log, TEXT("Request succeeded: %s"), *response->GetContentAsString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Request failed"));
			}
		}
	);
}

void ACSWGameMode::AppendHitLog(const TMap<FString, struct TTuple<int32, float>>& hitLog)
{
	for (auto nde : hitLog)
	{
		HitLog[nde.Key].Get<0>() += nde.Value.Get<0>();
		HitLog[nde.Key].Get<1>() += nde.Value.Get<1>();
	}
}

void ACSWGameMode::AppendShootLog(const FString& id, int shootingCnt)
{
	ShootLog[id] += shootingCnt;
}
