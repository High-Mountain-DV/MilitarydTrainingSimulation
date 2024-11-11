// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CSWGameMode.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FUserLog
{
	GENERATED_BODY()

	float damageDealt;
	int32 assist;
	int32 kill;
	int32 hitBullet;
	int32 shootBullet;
	int32 awareness;
	
	FUserLog()
		: damageDealt(0), assist(0), kill(0), hitBullet(0), shootBullet(0), awareness(0)
	{
	}

	float GetAccuracy() const
	{
		return shootBullet == 0 ? 0 : (float)hitBullet / (float)shootBullet;
	}
};

USTRUCT(BlueprintType)
struct FGameLog
{
	GENERATED_BODY()

	int32 injuredPlayer;
	int32 deadPlayer;
	float playTime;

	FGameLog()
		: injuredPlayer(0), deadPlayer(0), playTime(0)
	{
	}
};

UCLASS()
class VRMILITARYSIMULATION_API ACSWGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void CompleteOnePlayerLoading(UMaterialInstanceDynamic* CamMtl, const FString& nickname);
	
	void AppendHitLog(const TMap<FString, struct TTuple<int32, float>>& hitLog, const FString& killer = "");

	UFUNCTION(BlueprintCallable)
	void AppendShootLog(const FString& id, int shootingCnt);

	void AppendAwareLog(const TArray<FString>& encounter, const TArray<FString>& damaged);

	UFUNCTION(BlueprintCallable)
	void EndGame();

	void OnCompleteEndGame();
	
	void CollectPlayerLog();
	void CollectEnemyLog();
	
	void PostCombatLog(const FString& nickname, const FUserLog& userLog);
private:
	UPROPERTY()
	class AHttpActor* HttpActor;

	TMap<FString, FUserLog> UserLogs;
	FGameLog GameLog;

	int32 EndPlayerCnt = 0;
	int32 DeadPlayerCnt = 0;
	int32 DeadEnemyCnt = 0;
	int32 EnemyCnt = 0;
	int32 PlayerCnt = 0;
	int32 PlayTime;
};
