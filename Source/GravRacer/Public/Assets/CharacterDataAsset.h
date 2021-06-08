// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterDataAsset.generated.h"

class UTexture2D;
class AGRPilotActor;
class USoundCue;

USTRUCT(BlueprintType)
struct FCharacterSkinData
{
	GENERATED_BODY()
public:

	//Icon
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> SkinIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText SkinName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<AGRPilotActor> PilotClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString ItemCode;

	FCharacterSkinData()
	{

	}

};

/**
 * 
 */
UCLASS()
class GRAVRACER_API UCharacterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	//Asset Type
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AssetManager)
	FPrimaryAssetType AssetType;

	//Gender
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsFemale;

	//Name
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText CharacterName;

	//Icon
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> CharacterIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundCue* CharacterSelected;

	//Skins list
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FCharacterSkinData> CharacterSkins;

	//Constructor
	UCharacterDataAsset();

	FPrimaryAssetId GetPrimaryAssetId() const;
	FString GetIdentifierString() const;

	UFUNCTION(BlueprintCallable)
	void SetSkinIcon(int32 SkinIndex, UTexture2D* IconTexture);

};
