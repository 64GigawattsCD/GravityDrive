// Copyright Epic Games, Inc. All Rights Reserved.

#include "GravRacer.h"
#include "GravRacerMenuWidgetStyle.h"

FGravRacerMenuStyle::FGravRacerMenuStyle()
{
}

FGravRacerMenuStyle::~FGravRacerMenuStyle()
{
}

const FName FGravRacerMenuStyle::TypeName(TEXT("FGravRacerMenuStyle"));

const FGravRacerMenuStyle& FGravRacerMenuStyle::GetDefault()
{
	static FGravRacerMenuStyle Default;
	return Default;
}

void FGravRacerMenuStyle::GetResources(TArray<const FSlateBrush*>& OutBrushes) const
{
}


UGravRacerMenuWidgetStyle::UGravRacerMenuWidgetStyle( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
{
	
}
