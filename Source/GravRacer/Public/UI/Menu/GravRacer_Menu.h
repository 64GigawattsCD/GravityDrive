// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GravRacer_Menu.generated.h"

class AController;

UCLASS()
class AGravRacer_Menu : public AGameModeBase
{
	GENERATED_UCLASS_BODY()

	/** skip it, menu doesn't require player start or pawn */
	virtual void RestartPlayer(AController* NewPlayer) override;
};
