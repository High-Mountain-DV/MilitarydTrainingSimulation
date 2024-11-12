// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SG_GrenadeManager.generated.h"

/**
 * 
 */
UCLASS()
class VRMILITARYSIMULATION_API USG_GrenadeManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// 수류탄 던지기 시도
	bool TryThrowGrenade(bool& OutFlag);

private:
	static const int32 MAX_GRENADE_THROWS = 2;
	int32 CurrentGrenadeCount = 0;

};
