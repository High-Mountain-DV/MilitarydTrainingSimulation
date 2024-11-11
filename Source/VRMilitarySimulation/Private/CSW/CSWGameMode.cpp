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
	GameLog.playTime = GetWorld()->GetTimeSeconds();
}

void ACSWGameMode::CompleteOnePlayerLoading(UMaterialInstanceDynamic* CamMtl, const FString& nickname)
{
	auto* CommenderScreen = Cast<ACommenderScreen>(UGameplayStatics::GetActorOfClass(GetWorld(), ACommenderScreen::StaticClass()));
	if (CommenderScreen)
		CommenderScreen->AddPlayerScreen(CamMtl);
	UserNicknames.Add(nickname);
}

void ACSWGameMode::EndGame()
{
	CollectPlayerLog();
	CollectEnemyLog();
	GameLog.playTime = GetWorld()->GetTimeSeconds() - GameLog.playTime;
	for (auto nickname : UserNicknames)
		PostCombatLog(nickname);


	//tmp
	auto *gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());

	if (gi)
	{
		gi->StopRecording();
		gi->ExitSession();
		gi->GoReportRoom();
	}
}

void ACSWGameMode::OnCompleteEndGame()
{
	if (++EndPlayerCnt == UserNicknames.Num())
	{
		auto *gi = Cast<UCSWGameInstance>(GetWorld()->GetGameInstance());

		if (gi)
		{
			gi->StopRecording();
			gi->ExitSession();
			gi->GoReportRoom();
		}
	}
}

void ACSWGameMode::CollectPlayerLog()
{
	TArray<AActor*> outActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerVRCharacter::StaticClass(), outActors);
	for (AActor* actor : outActors)
	{
		APlayerVRCharacter* player = Cast<APlayerVRCharacter>(actor);

		FString nickname = player->GetActorLabel();
		int shootingCnt = player->GetShootingCnt();

		if (auto userLog = UserLogs.Find(nickname))
		{
			userLog->shootBullet += shootingCnt;
		}
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
		for (TMap<FString, TTuple<int32, float>>::TConstIterator it(hitLog); it; ++it)
		{
			FString nickname = it->Key;
			int32 hitCnt = it->Value.Get<0>();
			float hitDamage = it->Value.Get<1>();

			if (auto userLog = UserLogs.Find(nickname))
			{
				userLog->hitBullet += hitCnt;
				userLog->damageDealt += hitDamage;
			}
		}
	}
}



void ACSWGameMode::PostCombatLog(const FString& nickname)
{
	AHttpActor actor;
	
	TMap<FString, FString> header;
	TMap<FString, FString> body;

	//make header
	auto gi = Cast<UCSWGameInstance>(GetGameInstance());
	header.Add("Content-Type","application/json");
	header.Add("Authorization", gi->GetUserToken());

	// make body
	TSharedPtr<FJsonObject> jsonObject = MakeShareable(new FJsonObject());

	auto userLog = UserLogs.Find(nickname); 

	jsonObject->SetNumberField("damage_dealt", userLog->damageDealt);
	jsonObject->SetNumberField("assist", userLog->assist);
	jsonObject->SetNumberField("play_time", GameLog.playTime);
	jsonObject->SetNumberField("accuracy", userLog->GetAccuracy());
	jsonObject->SetNumberField("score", 0);
	jsonObject->SetStringField("nickname", nickname);
	jsonObject->SetNumberField("awareness", userLog->awareness);
	jsonObject->SetNumberField("ally_injuries", GameLog.injuredPlayer);
	jsonObject->SetNumberField("ally_deaths", GameLog.deadPlayer);
	jsonObject->SetNumberField("kills", userLog->kill);

	FString json;
	TSharedRef<TJsonWriter<TCHAR>> writer = TJsonWriterFactory<TCHAR>::Create(&json);
	FJsonSerializer::Serialize(jsonObject.ToSharedRef(), writer);
	
	UE_LOG(LogTemp, Warning, TEXT("%s"), *UJsonParseLib::MakeJson(body));
	//post
	actor.Request(
		"/api/combat",
		"POST",
		header,
		UJsonParseLib::MakeJson(body),
		[this](FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
		{
			if (bWasSuccessful && response.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("Request succeeded: %s"), *response->GetContentAsString());
				OnCompleteEndGame();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Request failed"));
			}
		}
	);
}

void ACSWGameMode::AppendHitLog(const TMap<FString, struct TTuple<int32, float>>& hitLog, const FString& killer)
{
	for (TMap<FString, TTuple<int32, float>>::TConstIterator it(hitLog); it; ++it)
	{
		FString nickname = it.Key();
		auto* userLog = UserLogs.Find(nickname);

		if (userLog)
		{
			userLog->hitBullet += it->Value.Get<0>();
			userLog->damageDealt += it->Value.Get<1>();
			if (killer == nickname)
				userLog->kill++;
			else
				userLog->assist++;
		}
	}
}

void ACSWGameMode::AppendShootLog(const FString& nickname, int shootingCnt)
{
	auto* userLog = UserLogs.Find(nickname);

	userLog->shootBullet += shootingCnt;
}

void ACSWGameMode::AppendAwareLog(const TArray<FString>& encounter, const TArray<FString>& damaged)
{
	TMap<FString, int> awareMap;

	for (int i = 0; i < encounter.Num(); i++)
	{
		awareMap[encounter[i]]++;
	}
	for (int i = 0; i < encounter.Num(); i++)
	{
		awareMap[damaged[i]]++;
	}

	for (auto nde : awareMap)
	{
		if (nde.Value == 2)
			UserLogs[nde.Key].awareness++;
	}
}
