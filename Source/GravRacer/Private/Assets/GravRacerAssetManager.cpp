// Copyright Epic Games, Inc. All Rights Reserved.

#include "GravRacerAssetManager.h"
#include "Assets/CharacterDataAsset.h"
#include "Assets/VehicleDataAsset.h"
#include "AbilitySystemGlobals.h"

const FPrimaryAssetType	UGravRacerAssetManager::CharacterDataType = TEXT("Character");
const FPrimaryAssetType	UGravRacerAssetManager::VehicleDataType = TEXT("Vehicle");
/*
const FPrimaryAssetType	UGravRacerAssetManager::PotionItemType = TEXT("Potion");
const FPrimaryAssetType	UGravRacerAssetManager::SkillItemType = TEXT("Skill");
const FPrimaryAssetType	UGravRacerAssetManager::TokenItemType = TEXT("Token");
const FPrimaryAssetType	UGravRacerAssetManager::WeaponItemType = TEXT("Weapon");*/

UGravRacerAssetManager& UGravRacerAssetManager::Get()
{
	UGravRacerAssetManager* This = Cast<UGravRacerAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		//UE_LOG(LogActionGravRacer, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be GravRacerAssetManager!"));
		return *NewObject<UGravRacerAssetManager>(); // never calls this
	}
}

void UGravRacerAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}


UCharacterDataAsset* UGravRacerAssetManager::ForceLoadCharacter(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{	
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UCharacterDataAsset* LoadedItem = Cast<UCharacterDataAsset>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}


UVehicleDataAsset* UGravRacerAssetManager::ForceLoadVehicle(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UVehicleDataAsset* LoadedItem = Cast<UVehicleDataAsset>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}