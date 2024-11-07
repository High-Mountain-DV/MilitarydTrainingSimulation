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
	void CompleteOnePlayerLoading(UMaterialInstanceDynamic* CamMtl);

	UFUNCTION(BlueprintCallable)
	void EndGame();

};
