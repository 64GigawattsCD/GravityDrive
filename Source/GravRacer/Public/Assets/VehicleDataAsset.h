// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VehicleDataAsset.generated.h"

class UTexture2D;
class UStaticMesh;

USTRUCT(BlueprintType)
struct FVehicleSkinData
{
	GENERATED_BODY()
public:

	//Icon
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> SkinIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* VehicleMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ItemCode;

	FVehicleSkinData()
	{

	}

};

/**
 *
 */
UCLASS()
class GRAVRACER_API UVehicleDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	//Asset Type
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AssetManager)
	FPrimaryAssetType AssetType;

	//Name
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText VehicleName;

	//Icon
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> VehicleIcon;

	//Skins list
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FVehicleSkinData> VehicleSkins;

	//TODO
	//UPROPERTY(EditAnywhere, BlueprintReadOnly)
	//Special move

	//Constructor
	UVehicleDataAsset();

	FPrimaryAssetId GetPrimaryAssetId() const;
	FString GetIdentifierString() const;
};
