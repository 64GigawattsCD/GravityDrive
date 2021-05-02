// Copyright Epic Games, Inc. All Rights Reserved.

#include "GravRacerMenuItemWidgetStyle.h"

FGravRacerMenuItemStyle::FGravRacerMenuItemStyle()
{
}

FGravRacerMenuItemStyle::~FGravRacerMenuItemStyle()
{
}

const FName FGravRacerMenuItemStyle::TypeName(TEXT("FGravRacerMenuItemStyle"));

const FGravRacerMenuItemStyle& FGravRacerMenuItemStyle::GetDefault()
{
	static FGravRacerMenuItemStyle Default;
	return Default;
}

void FGravRacerMenuItemStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
	OutBrushes.Add(&BackgroundBrush);
	OutBrushes.Add(&HighlightBrush);

	ControlsListTableRowStyle.GetResources(OutBrushes);
}


UGravRacerMenuItemWidgetStyle::UGravRacerMenuItemWidgetStyle( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	
}
