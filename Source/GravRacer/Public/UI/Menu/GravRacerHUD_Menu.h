// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GravRacerTypes.h"
#include "GameFramework/HUD.h"
#include "GravRacerHUD_Menu.generated.h"

class SGravRacerMenuWidget;
class SGravRacerControlsSetup;

namespace EGravRacerMenu
{
	enum Type
	{
		PlaySolo,
		HostLocal,
		JoinLocal,
		Map1,
		Map2,
		RemapControls,
		RemapController,
		ChangeQuality,	
		Quit,
	};
};

UCLASS()
class AGravRacerHUD_Menu : public AHUD
{
	GENERATED_UCLASS_BODY()

	/** in game menu pointer */
	MenuPtr MainMenu;

	/** controls settings menu */
	MenuPtr ControlsMenu;

	/** called just after game begins */
	virtual void PostInitializeComponents() override;

	/** Clears out the old widgets, rebuilds them */
	void RebuildWidgets(bool bHotReload = false);

private:

	/** main menu widget pointer */
	TSharedPtr<SGravRacerMenuWidget> MenuWidget;

	/** controls setup widget pointer */
	TSharedPtr<SGravRacerControlsSetup> ControlsSetupWidget;

	/** graphics quality menu item */
	FGravRacerMenuItem* QualityMenuItem;

	/** quality descriptions list */
	TArray<FText> LowHighList;

	/** menu callback */
	void ExecuteMenuAction(EGravRacerMenu::Type Action);

	/** starts the single player game */
	void PlaySolo();

	/** hosts local multiplayer game */
	void HostLocal();

	/** quits the game */
	void Quit();

	/** Display the a loading screen for the given action if it is required. */
	void ShowLoadingScreen();		
};
