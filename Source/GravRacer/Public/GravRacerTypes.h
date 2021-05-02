// Copyright Epic Games, Inc. All Rights Reserved.


#include "CoreMinimal.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Materials/MaterialInterface.h"
#include "GravRacerTypes.generated.h"
#pragma once

class UGravRacerSaveGame;
typedef TSharedPtr<TArray<class FGravRacerMenuItem>> MenuPtr;

#define ACTOR_ROLE_FSTRING *(FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true)->GetNameStringByValue(GetLocalRole()))
#define GET_ACTOR_ROLE_FSTRING(Actor) *(FindObject<UEnum>(ANY_PACKAGE, TEXT("ENetRole"), true)->GetNameStringByValue(Actor->GetLocalRole()))

#define COLLISION_ABILITY						ECollisionChannel::ECC_GameTraceChannel1
#define COLLISION_PROJECTILE					ECollisionChannel::ECC_GameTraceChannel2
#define COLLISION_ABILITYOVERLAPPROJECTILE		ECollisionChannel::ECC_GameTraceChannel3
#define COLLISION_PICKUP						ECollisionChannel::ECC_GameTraceChannel4
#define COLLISION_INTERACTABLE					ECollisionChannel::ECC_GameTraceChannel5

UENUM(BlueprintType)
enum class EGSAbilityInputID : uint8
{
	// 0 None
	None				UMETA(DisplayName = "None"),
	// 1 Confirm
	Confirm				UMETA(DisplayName = "Confirm"),
	// 2 Cancel
	Cancel				UMETA(DisplayName = "Cancel"),
	// 3 Sprint
	Sprint				UMETA(DisplayName = "Sprint"),
	// 4 Jump
	Jump				UMETA(DisplayName = "Jump"),
	// 5 PrimaryFire
	PrimaryFire			UMETA(DisplayName = "Primary Fire"),
	// 6 SecondaryFire
	SecondaryFire		UMETA(DisplayName = "Secondary Fire"),
	// 7 Alternate Fire
	AlternateFire		UMETA(DisplayName = "Alternate Fire"),
	// 8 Reload
	Reload				UMETA(DisplayName = "Reload"),
	// 9 NextWeapon
	NextWeapon			UMETA(DisplayName = "Next Weapon"),
	// 10 PrevWeapon
	PrevWeapon			UMETA(DisplayName = "Previous Weapon"),
	// 11 Interact
	Interact			UMETA(DisplayName = "Interact")
};

USTRUCT()
struct FKeybindingUIConfig
{
	GENERATED_BODY()

	/** action name if mapping action */
	UPROPERTY()
	FName ActionName;

	/** axis name if mapping axis to key */
	UPROPERTY()
	FName AxisName;

	/** Key to bind it to. */
	UPROPERTY()
	FKey Key;

	/** axis scale is required to find proper mapping */
	UPROPERTY()
	float Scale = 0.0f;

	/** name to display in user interface */
	UPROPERTY()
	FText DisplayName;

	/** group name */
	UPROPERTY()
	FName GroupName;
};


/** keep in sync with GravRacerImpactEffect, VehicleWheelEffect  */
/** This enum type is deprecated, so please do not use this **/
UENUM()
namespace EPhysMaterialType
{
	enum Type
	{
		Unknown,
		Asphalt,
		Dirt,
		Water,
		Wood,
		Stone,
		Metal,
	};
}

/** When you add new types, make sure you add to 
 *	[/Script/Engine.PhysicsSettings] section DefaultEngine.INI 
 */
#define SURFACE_Default		SurfaceType_Default
#define SURFACE_Asphalt		SurfaceType1
#define SURFACE_Dirt		SurfaceType2
#define SURFACE_Water		SurfaceType3
#define SURFACE_Wood		SurfaceType4
#define SURFACE_Stone		SurfaceType5
#define SURFACE_Metal		SurfaceType6
#define SURFACE_Grass		SurfaceType7
#define SURFACE_Gravel		SurfaceType8
#define SURFACE_Concrete	SurfaceType9
#define SURFACE_Flesh		SurfaceType10

USTRUCT(BlueprintType)
struct FDecalData
{
	GENERATED_BODY()

	/** material */
	UPROPERTY(EditDefaultsOnly, Category=Decal)
	UMaterialInterface* DecalMaterial;

	/** quad size (width & height) */
	UPROPERTY(EditDefaultsOnly, Category=Decal)
	float DecalSize;

	/** lifespan */
	UPROPERTY(EditDefaultsOnly, Category=Decal)
	float LifeSpan;

	/** defaults */
	FDecalData()
		: DecalMaterial(nullptr)
		, DecalSize(256.f)
		, LifeSpan(10.f)
	{
	}
};

USTRUCT(BlueprintType)
struct FImpactEffectData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Visual)
	UParticleSystem* ParticleFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	USoundCue* SoundFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	FDecalData DecalFX;

	FImpactEffectData()
	{

	}

	FImpactEffectData(UParticleSystem* ParticleFX, USoundCue* SoundFX, FDecalData DecalFX)
		:ParticleFX(ParticleFX)
		,SoundFX(SoundFX)
		, DecalFX(DecalFX)
	{

	}
};

/** replicated information on a hit we've taken */
USTRUCT(BlueprintType)
struct FTakeHitInfo
{
	GENERATED_BODY()

	/** The amount of damage actually applied */
	UPROPERTY()
	float ActualDamage;

	/** The damage type we were hit with. */
	UPROPERTY()
	UClass* DamageTypeClass;

	/** Who hit us */
	UPROPERTY()
	TWeakObjectPtr<class AGravRacerPawn> PawnInstigator;

	/** Who actually caused the damage */
	UPROPERTY()
	TWeakObjectPtr<class AActor> DamageCauser;

	/** Specifies which DamageEvent below describes the damage received. */
	UPROPERTY()
	int32 DamageEventClassID;

	/** Rather this was a kill */
	UPROPERTY()
	uint32 bKilled:1;

private:

	/** A rolling counter used to ensure the struct is dirty and will replicate. */
	UPROPERTY()
	uint8 EnsureReplicationByte;

	/** Describes general damage. */
	UPROPERTY()
	FDamageEvent GeneralDamageEvent;

	/** Describes point damage, if that is what was received. */
	UPROPERTY()
	FPointDamageEvent PointDamageEvent;

	/** Describes radial damage, if that is what was received. */
	UPROPERTY()
	FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo();

	FDamageEvent& GetDamageEvent();
	void SetDamageEvent(const FDamageEvent& DamageEvent);
	void EnsureReplication();
};

/** Delegate called when the save game has been loaded/reset */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoaded, UGravRacerSaveGame*, SaveGame);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoadedNative, UGravRacerSaveGame*);
