// Copyright 2020 Dan Kestranek.


#include "Assets/VehicleDataAsset.h"
#include "Assets/GravRacerAssetManager.h"

UVehicleDataAsset::UVehicleDataAsset()
{
	AssetType = UGravRacerAssetManager::VehicleDataType;
}

FPrimaryAssetId UVehicleDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(AssetType, GetFName());
}

FString UVehicleDataAsset::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}
