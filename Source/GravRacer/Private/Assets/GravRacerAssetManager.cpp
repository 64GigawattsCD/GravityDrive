// Copyright Epic Games, Inc. All Rights Reserved.

#include "GravRacerAssetManager.h"
//#include "Items/GravRacerItem.h"
#include "AbilitySystemGlobals.h"

const FPrimaryAssetType	UGravRacerAssetManager::RacerDataType = TEXT("Racer");
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

/*
UGravRacerItem* UGravRacerAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{	
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UGravRacerItem* LoadedItem = Cast<UGravRacerItem>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogActionGravRacer, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}*/