// Copyright Epic Games, Inc. All Rights Reserved.

//#include "GravRacer.h"
#include "Player/GravRacerPlayerCameraManager.h"

AGravRacerPlayerCameraManager::AGravRacerPlayerCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bUseClientSideCameraUpdates = false;
	bAlwaysApplyModifiers = true;
}

