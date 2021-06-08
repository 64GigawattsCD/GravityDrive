// Copyright Craig Delancy 2019

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/Texture2D.h"
#include "TerraForm_Runtime/Public/Assets/GISProjectionAsset.h"
#include "TerraForm_Runtime/Public/TerraFormTypes.h"
#include "GISWorldAsset.generated.h"

//This is a companion file for raster Geographic data that helps to place the image within a projected coordinate system
UCLASS(BlueprintType, hidecategories=Object)
class TERRAFORM_RUNTIME_API UGISWorldAsset : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GISData")
	FWorldFileData WorldData;

	//List of corresponding raster image data
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GISData")
	TArray<UTexture2D*> RasterData;

	//The projection system of the raster image data
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GISData")
	UGISProjectionAsset* ProjectionSystem;

	//Saving the file contents for later use
	FString FileContents;
};
