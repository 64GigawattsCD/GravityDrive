// Copyright Epic Games, Inc. All Rights Reserved.

#include "GravRacerMenuSoundsWidgetStyle.h"

FGravRacerMenuSoundsStyle::FGravRacerMenuSoundsStyle()
{
}

FGravRacerMenuSoundsStyle::~FGravRacerMenuSoundsStyle()
{
}

const FName FGravRacerMenuSoundsStyle::TypeName(TEXT("FGravRacerMenuSoundsStyle"));

const FGravRacerMenuSoundsStyle& FGravRacerMenuSoundsStyle::GetDefault()
{
	static FGravRacerMenuSoundsStyle Default;
	return Default;
}

void FGravRacerMenuSoundsStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
}


UGravRacerMenuSoundsWidgetStyle::UGravRacerMenuSoundsWidgetStyle( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	
}
