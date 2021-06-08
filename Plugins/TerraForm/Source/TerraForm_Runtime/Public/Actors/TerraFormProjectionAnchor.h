// Copyright Craig Delancy 2019

#pragma once

#include "CoreMinimal.h"
#include "TerraForm_Runtime/Public/TerraFormTypes.h"
#include "Actors/TerraFormPointActor.h"
#include "GameFramework/Actor.h"

THIRD_PARTY_INCLUDES_START
//#include "GDAL/Includes/gdal_priv.h"
THIRD_PARTY_INCLUDES_END

#include "TerraFormProjectionAnchor.generated.h"

class UBillboardComponent;
class UGISProjectionAsset;

UCLASS(Blueprintable)
class TERRAFORM_RUNTIME_API ATerraFormProjectionAnchor : public ATerraFormPointActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coordinate")
	UGISProjectionAsset* CoordinateSystem;

#if WITH_EDITOR 
	UStaticMeshComponent* PlaneMesh;
#endif

protected:

	OGRPoint Point;
	
public:	
	// Sets default values for this actor's properties
	ATerraFormProjectionAnchor(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR 
	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;
	virtual void EditorApplyTranslation(const FVector & DeltaTranslation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;
#endif

	//Esentially, the -Y axis of the actor
	UFUNCTION(BlueprintCallable, Category = "GIS")
	FVector GetNorthAxis() const;

	//Essentially, the +X axis of the actor
	UFUNCTION(BlueprintCallable, Category = "GIS")
	FVector GetEastAxis() const;

	//Get the Geographic coordinate of this point
	UFUNCTION(BlueprintCallable, Category = "GIS")
	virtual FVector2D GetGeographicCoordinate(float & OutElevation);

	//Get the projected coordinate of this point
	UFUNCTION(BlueprintCallable, Category = "GIS")
	virtual FVector2D GetProjectedCoordinate(float & OutElevation);

	//Input a coordinate in the same projection, returns the world-space location relative to this anchor, assumes east, north ordering
	UFUNCTION(BlueprintCallable, Category = "GIS")
	FVector GetWorldLocationFromCoordinate(FVector2D InProjectedCoordinate, float InElevation);
	FVector GetWorldLocationFromCoordinate(FVector InProjectedCoordinate);

	UFUNCTION(BlueprintCallable, Category = "GIS")
	FVector2D GetProjectedCoordinateFromWorldLocation(FVector InWorldLoc, float & OutElevation);
	FVector GetProjectedCoordinateFromWorldLocation(FVector InWorldLoc);

	//Return the linear units of the projection with an out parameter for the unit scale, defaults to Meters, 1.f
	UFUNCTION(BlueprintCallable, Category = "GIS")
	ETerraFormLinearUnit GetProjectionLinearUnit(float& UnitScale);

	//Returns the spatial reference being used by this projection anchor
	OGRSpatialReference* GetSpatialReference();
};
