// Copyright Craig Delancy 2019

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TerraFormEditorFunctionLibrary.generated.h"

class UTexture2D;

UCLASS()
class TERRAFORM_EDITORASSETS_API UTerraFormEditorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	
public:

	//Take an existing texture, export to temp directory, gdal warp to new file, delete temp file, return name of new file for import
	static FString GDALWarpTexture(UTexture2D* Texture, FString Prefix);
	
};
