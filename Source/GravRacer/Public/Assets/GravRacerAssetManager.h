// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GravRacer.h"
#include "Engine/AssetManager.h"
#include "GravRacerAssetManager.generated.h"

class UGravRacerItem;

/**
 * Game implementation of asset manager, overrides functionality and stores game-specific types
 * It is expected that most games will want to override AssetManager as it provides a good place for game-specific loading logic
 * This is used by setting AssetManagerClassName in DefaultEngine.ini
 */
UCLASS()
class GRAVRACER_API UGravRacerAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGravRacerAssetManager() {}
	virtual void StartInitialLoading() override;

	/** Static types for data assets */
	static const FPrimaryAssetType	RacerDataType;
	static const FPrimaryAssetType	VehicleDataType;

	/** Static types for items */
	//static const FPrimaryAssetType	PotionItemType;
	//static const FPrimaryAssetType	SkillItemType;
	//static const FPrimaryAssetType	TokenItemType;
	//static const FPrimaryAssetType	WeaponItemType;

	/** Returns the current AssetManager object */
	static UGravRacerAssetManager& Get();

	/**
	 * Synchronously loads an GravRacerItem subclass, this can hitch but is useful when you cannot wait for an async load
	 * This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	 *
	 * @param PrimaryAssetId The asset identifier to load
	 * @param bDisplayWarning If true, this will log a warning if the item failed to load
	 */
	//UGravRacerItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);
};

