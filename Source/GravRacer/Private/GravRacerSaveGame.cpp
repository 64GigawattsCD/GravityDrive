// Copyright Epic Games, Inc. All Rights Reserved.

#include "GravRacerSaveGame.h"
//#include "GravRacerGameInstanceBase.h"

void UGravRacerSaveGame::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsLoading() && SavedDataVersion != EGravRacerSaveGameVersion::LatestVersion)
	{/*
		if (SavedDataVersion < EGravRacerSaveGameVersion::AddedItemData)
		{
			
			// Convert from list to item data map
			for (const FPrimaryAssetId& ItemId : InventoryItems_DEPRECATED)
			{
				InventoryData.Add(ItemId, FGravRacerItemData(1, 1));
			}

			InventoryItems_DEPRECATED.Empty();
		}*/
		
		SavedDataVersion = EGravRacerSaveGameVersion::LatestVersion;
	}
}