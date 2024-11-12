// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_GrenadeManager.h"

bool USG_GrenadeManager::TryThrowGrenade(bool& OutFlag)
{
	if (CurrentGrenadeCount < MAX_GRENADE_THROWS)
	{
		CurrentGrenadeCount++;

		if (CurrentGrenadeCount == MAX_GRENADE_THROWS)
			OutFlag = true;
		return true;
	}

	return false;
}
