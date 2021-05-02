// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

class AVehiclePlayerController;

//HUD widget base class
class SGravRacerHUDWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGravRacerHUDWidget)
	{}

	SLATE_ARGUMENT(TWeakObjectPtr<class UWorld>, OwnerWorld)

	SLATE_END_ARGS()

	/** Needed for every widget */
	void Construct(const FArguments& InArgs);

protected:
	EVisibility GetInfoTextVisibility() const;
	FSlateColor GetInfoTextColor() const;
	FText		GetInfoText() const;

	AVehiclePlayerController* PCOwner;

	/** Pointer to our parent World */
	TWeakObjectPtr<UWorld> OwnerWorld;
};


