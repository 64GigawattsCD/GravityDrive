// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/TerraFormSplineActor.h"
#include "TerraFormSplineMultiActor.generated.h"

/**
 * An actor that acts as a collection of other parallel spline actors
 */
UCLASS()
class TERRAFORM_RUNTIME_API ATerraFormSplineMultiActor : public ATerraFormSplineActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMultiActor")
	TSubclassOf<ATerraFormSplineActor> ChildSplineActorClass;

	UPROPERTY(Transient, BlueprintReadOnly, VisibleAnywhere, Category = "SplineMultiActor")
	TArray<ATerraFormSplineActor*> ChildSplineActors;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMultiActor")
	int32 NumChildren;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineMultiActor")
	float ChildSpacing;

	//Creates the child actors by offsetting the points of this spline laterally
	void CreateChildSplineActors();

public:
	virtual void OnConstruction(const FTransform & Transform) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

};
