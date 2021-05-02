// Copyright Epic Games, Inc. All Rights Reserved.

#include "GravRacer.h"
#include "UI/Style/GravRacerStyle.h"
#include "UI/Style/GravRacerMenuSoundsWidgetStyle.h"
#include "UI/Style/GravRacerMenuWidgetStyle.h"
#include "UI/Style/GravRacerMenuItemWidgetStyle.h"



class FGravRacerModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
		//Hot reload hack
		FSlateStyleRegistry::UnRegisterSlateStyle(FGravRacerStyle::GetStyleSetName());
		FGravRacerStyle::Initialize();
	}

	virtual void ShutdownModule() override
	{
		FGravRacerStyle::Shutdown();
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE( FGravRacerModule, GravRacer, "GravRacer" );

DEFINE_LOG_CATEGORY(LogVehicle)
