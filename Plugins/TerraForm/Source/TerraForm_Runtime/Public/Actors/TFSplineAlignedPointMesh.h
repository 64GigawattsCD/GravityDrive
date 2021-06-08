// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/TerraFormPointMesh.h"
#include "TFSplineAlignedPointMesh.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFORM_RUNTIME_API ATFSplineAlignedPointMesh : public ATerraFormPointMesh
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditInstanceOnly, Category = "PointActor")
	class ATerraFormSplineActor* SplineActor;

	//If true, rotation along spline will be yaw only
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PointActor")
	bool StayUpright;

private:

	//Component in charge of aligning the actor to the spline
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "PointActor")
	class UTerraFormSplineMovementComponent* SplineMovementComponent;

public:

	/** Returns SplineMovement subobject **/
	FORCEINLINE class UTerraFormSplineMovementComponent* GetSplineMovement() const { return SplineMovementComponent; }

	/** Name of the CharacterMovement component. Use this name if you want to use a different class (with ObjectInitializer.SetDefaultSubobjectClass). */
	static FName SplineMovementComponentName;

public:

	ATFSplineAlignedPointMesh(const FObjectInitializer& ObjectInitializer);

	//Set the spline actor and set the distance along the spline by finding the closest point on the spline
	UFUNCTION(BlueprintCallable, Category = "SplineAlignedPointMesh")
	virtual void LinkToSplineActor(class ATerraFormSplineActor* NewSplineActor);

	UFUNCTION(BlueprintCallable, Category = "SplineAlignedPointMesh")
	void SetSplineOffset(FVector NewOffset, ETerraFormLinearUnit Units = ETerraFormLinearUnit::LU_CM);
	
	UFUNCTION(BlueprintCallable, Category = "SplineAlignedPointMesh")
	FVector GetSplineOffset(ETerraFormLinearUnit Units = ETerraFormLinearUnit::LU_CM);
};
