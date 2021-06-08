// Copyright Craig Delancy 2020

#pragma once

#include "CoreMinimal.h"
#include "TerraFormTypes.h"
#include "Actors/TerraFormShapeDataActor.h"
#include "TerraFormSplineActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class TERRAFORM_RUNTIME_API ATerraFormSplineActor : public ATerraFormShapeDataActor
{
	GENERATED_BODY()

public:

	//Main spline component of the actor
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SplineActor")
	class UTFSnappingSplineComponent* Spline;

	//List of per-point attribute sets
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "SplineActor|Attributes")
	TArray<FTFAttributeSet> PointAttributes;
	
public:
	//Constructor
	ATerraFormSplineActor(const FObjectInitializer& ObjectInitializer);

	//Overridden to process attribute data once components are set up
	virtual void PostInitializeComponents() override;

	//Clear the spline points and the per-point attribute data
	void ClearSpline();

	//Add a spline point in world space paired with a set of attributes for the point
	void AddSplinePointWS(FVector WorldPoint, FTFAttributeSet PointAttributeSet);

	//Called to update the spline actor once the spline has been modified
	virtual void UpdateSpline();

	//Used to set attribute data, and will also call ProcessAttributesList
	UFUNCTION(BlueprintCallable, Category = "SplineActor|Attributes")
	virtual void ImportAttributes(TArray<FTFAttributeSet> InAttributes);

	virtual void GetSupportedAttributeNames_Implementation(TArray<FName>& StringNames, TArray<FName>& FloatNames, TArray<FName>& IntegerNames, TArray<FName>& BoolNames) override;

	void RemovePointAttributes(int32 Index);
	void AddPointAttributes(int32 Index);

	//Find the index of the first point where the given attribute changes to a new value
	int32 FindPointOfAttributeChange(int32 StartAt, FName AttributeName, bool CurrentValue = false);
	int32 FindPointOfAttributeChange(int32 StartAt, FName AttributeName, float CurrentValue = 0.f);
	int32 FindPointOfAttributeChange(int32 StartAt, FName AttributeName, FString CurrentValue = "NotFound");
	int32 FindPointOfAttributeChange(int32 StartAt, FName AttributeName, int32 CurrentValue = -1);

	UFUNCTION(BlueprintCallable, Category = "SplineActor|Attributes")
	bool GetBoolAttributeAtDistanceAlongSpline(FName AttributeName, float Distance);
	UFUNCTION(BlueprintCallable, Category = "SplineActor|Attributes")
	float GetFloatAttributeAtDistanceAlongSpline(FName AttributeName, float Distance, bool Interpolate = true);
	UFUNCTION(BlueprintCallable, Category = "SplineActor|Attributes")
	FString GetStringAttributeAtDistanceAlongSpline(FName AttributeName, float Distance);
	UFUNCTION(BlueprintCallable, Category = "SplineActor|Attributes")
	int32 GetIntegerAttributeAtDistanceAlongSpline(FName AttributeName, float Distance);

/*
#if WITH_EDITOR  
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChanged) override;
	virtual void PreEditChange(class FEditPropertyChain& PropertyAboutToChange) override;

#endif*/
	
};
