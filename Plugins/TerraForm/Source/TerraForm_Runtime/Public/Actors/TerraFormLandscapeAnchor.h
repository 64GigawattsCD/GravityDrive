// Copyright Craig Delancy 2020

#pragma once

#include "CoreMinimal.h"
#include "Actors/TerraFormProjectionAnchor.h"
#include "TerraFormLandscapeAnchor.generated.h"

class ALandscape;

//Projection anchor meant to be the immediate parent of a geo-referenced landscape
//Able to quickly supply useful information such as the landscape size and corner coordinates

UCLASS(Blueprintable)
class TERRAFORM_RUNTIME_API ATerraFormLandscapeAnchor : public ATerraFormProjectionAnchor
{
	GENERATED_BODY()

public:
	//UPROPERTY(BlueprintReadOnly, Category = "Landscape")
	//ALandscape* LandscapeActor;

	UPROPERTY(BlueprintReadOnly, Category = "Landscape")
	FVector2D LandscapeSize;
	
	//if Projected = true, returns corner coordinates in the projected coordinate system; if false, it returns the unreal coordinates.
	UFUNCTION(BlueprintCallable, Category = "Landscape")
	virtual bool GetLandscapeCorners(TArray<FVector2D> &CornerCoordinates, bool Projected = true);
};
