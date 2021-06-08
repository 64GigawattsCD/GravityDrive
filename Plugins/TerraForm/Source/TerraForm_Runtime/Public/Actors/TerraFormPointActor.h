// Copyright Craig Delancy 2020

#pragma once

#include "CoreMinimal.h"
#include "TerraFormTypes.h"
#include "Actors/TerraFormShapeDataActor.h"
#include "TerraFormPointActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TERRAFORM_RUNTIME_API ATerraFormPointActor : public ATerraFormShapeDataActor
{
	GENERATED_BODY()

public:

	//Cordinate of this point within the projected coordinate system, typically projections are in meters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coordinate")
	FVector2D ProjectedCoordinate;

	//Elevation in meters above sea level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coordinate")
	float Elevation;
	
	ATerraFormPointActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PointActor|Attributes")
	FTFAttributeSet ActorAttributes;

	//Disable this if setting scale and rotation from attribute data is undesirable
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PointActor|Attributes")
	bool UseScaleAndRotationAttributes;

public:

	virtual void PostInitializeComponents() override;

	//packs coordinate and elevation together into a vector with no conversions or scaling applied
	FVector GetCoordinateAsFVector();

	//Moves the actor within the world based on its coordinates withing the projection system
	UFUNCTION(BlueprintCallable, Category = "Coordinate")
	virtual void UpdateLocationFromProjection();

	//Updates the coordinates within the projection system from its location in the world
	UFUNCTION(BlueprintCallable, Category = "Coordinate")
	virtual void UpdateProjectionCoordinateFromLocation();

	UFUNCTION(BlueprintCallable, Category = "PointActor|Attributes")
	virtual void ImportAttributes(FTFAttributeSet InAttributes);

	//Event for processing imported attribute data. Return true if data was actually used to invoke a change in the actor's state.
	virtual void ProcessAttributesList() override;

	virtual void GetSupportedAttributeNames_Implementation(TArray<FName>& StringNames, TArray<FName>& FloatNames, TArray<FName>& IntegerNames, TArray<FName>& BoolNames) override;
};
