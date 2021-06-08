// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TerraForm_Runtime/Public/TerraFormTypes.h"
#include "TFAttributeSettingsAsset.generated.h"

UENUM(BlueprintType)
enum class ETFAttributeListMode : uint8
{
	AL_None		UMETA(DisplayName = "None"),
	AL_Black 	UMETA(DisplayName = "Black List"),
	AL_White 	UMETA(DisplayName = "White List")
};

USTRUCT(BlueprintType)
struct FTFAttributeRescale
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	float InMinimum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	float InMaximum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	float OutMinimum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	float OutMaximum;

	FTFAttributeRescale()
	{
		InMinimum = 0.f;
		InMaximum = 1.f;
		OutMinimum = 0.f;
		OutMaximum = 1.f;
	}
};

/**
 * Class that saves a set of options to use with GIS shape data attribute field importing
 * Otherwise, users would have to keep re-entering a s-load of option for every file
 */

UCLASS(Blueprintable)
class TERRAFORM_RUNTIME_API UTFAttributeSettingsAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:

	//Controls how the atrribute list is applied to an incoming attribute set.
	//Blacklist- Removes fields from the attribute list before applying
	//Whitelist- Only passes on fields that have their names listed
	//None- No filtering is applied to the incoming attribute set
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	ETFAttributeListMode AttributeListMode;

	//List of attribute field names to use in filtering incoming attribute sets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	TArray<FName> AttributeList;

	//Used to remap fields during import
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	TMap<FName, FTFAttributeRescale> RemappingList;

	//Used to rename fields during import
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TerraFormAttributes")
	TMap<FName, FName> RenamingList;


};
