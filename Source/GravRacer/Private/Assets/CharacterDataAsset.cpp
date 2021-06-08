// Copyright 2020 Dan Kestranek.


#include "Assets/CharacterDataAsset.h"
#include "Assets/GravRacerAssetManager.h"
#include "Engine/Texture2D.h"
#include "Pawns/GRPilotActor.h"

UCharacterDataAsset::UCharacterDataAsset()
{
	AssetType = UGravRacerAssetManager::CharacterDataType;
}

FPrimaryAssetId UCharacterDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(AssetType, GetFName());
}

FString UCharacterDataAsset::GetIdentifierString() const
{
	return GetPrimaryAssetId().ToString();
}

void UCharacterDataAsset::SetSkinIcon(int32 SkinIndex, UTexture2D* IconTexture)
{
#if WITH_EDITOR
	if (SkinIndex < 0 || SkinIndex >= CharacterSkins.Num())
	{
		return;
	}

	CharacterSkins[SkinIndex].SkinIcon = IconTexture;

	MarkPackageDirty();
#endif
}
