// Copyright Craig Delancy 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TerraForm_Runtime/Public/TerraFormTypes.h"


THIRD_PARTY_INCLUDES_START
#include "GDAL/Includes/gdal_priv.h"
#include "GDAL/Includes/ogr_spatialref.h"
THIRD_PARTY_INCLUDES_END

#include "GISProjectionAsset.generated.h"

//This file defines a projected coordinate system
UCLASS(BlueprintType, hidecategories=Object)
class TERRAFORM_RUNTIME_API UGISProjectionAsset : public UObject
{
	GENERATED_BODY()
	
public:

	OGRSpatialReference* SpatRef;
	//true for well known text, false for proj4
	bool WKT;
	
	UPROPERTY(BlueprintReadOnly, Category = "GISProjection")
	FString FileContents;

	//UFUNCTION(BlueprintCallable, Category = "GISProjection")
	OGRSpatialReference* GetSpatialReference();

};
