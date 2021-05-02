// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Menu/GravRacer_Menu.h"
#include "UI/Menu/GravRacerHUD_Menu.h"

AGravRacer_Menu::AGravRacer_Menu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	HUDClass = AGravRacerHUD_Menu::StaticClass();
}

void AGravRacer_Menu::RestartPlayer(AController* NewPlayer)
{
	// don't restart
}
