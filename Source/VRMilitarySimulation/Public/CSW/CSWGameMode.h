// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CSWGameMode.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API ACSWGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void CompleteOnePlayerLoading(UMaterialInstanceDynamic* CamMtl, FString id);


	void AppendHitLog(const TMap<FString, struct TTuple<int32, float>>& hitLog);

	void AppendShootLog(const FString& id, int shootingCnt);

	UFUNCTION(BlueprintCallable)
	void EndGame();
	
	void CollectPlayerLog();
	void CollectEnemyLog();
	void PostCombatLog(const FString& id);
private:
	TArray<FString> UserIds;
	TMap<FString, TTuple<int32, float>> HitLog;
	TMap<FString, int> ShootLog;
};
