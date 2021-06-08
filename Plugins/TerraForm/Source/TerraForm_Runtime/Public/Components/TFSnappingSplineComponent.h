// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TerraFormTypes.h"
#include "Components/SplineComponent.h"
#include "TFSnappingSplineComponent.generated.h"



UCLASS()
class TERRAFORM_RUNTIME_API UTFSnappingSplineComponent : public USplineComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineConections")
	TArray<class ATFSplineConnectionActor*> SplineConnections;

	UTFSnappingSplineComponent();

#if WITH_EDITOR  
	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChanged) override;
	//virtual void PreEditChange(class FEditPropertyChain& PropertyAboutToChange) override;

#endif

	UFUNCTION(BlueprintCallable, Category = "SnappingSplineComponent")
	TArray<class ATFSplineConnectionActor*> GetConnectionsForSplineInRange(float StartDistance, float EndDistance);

	UFUNCTION(BlueprintCallable, Category = "SnappingSplineComponent")
	TArray<class ATFSplineConnectionActor*> GetConnectionsAtSplinePoint(int32 PointIndex);

	UFUNCTION(BlueprintCallable, Category = "SnappingSplineComponent")
	float EstimateDistanceAlongSplineFromWorldLocation(FVector WorldLocation);

protected:

	UPROPERTY()//Should it be transient?
	FSplineCurves PreEditSplineCurves;
};
