// Fill out your copyright notice in the Description page of Project Settings.


#include "SG_GrenadeManager.h"

bool USG_GrenadeManager::TryThrowGrenade()
{
	if (CurrentGrenadeCount < MAX_GRENADE_THROWS)
	{
		CurrentGrenadeCount++;
		return true;
	}

	return false;
}
