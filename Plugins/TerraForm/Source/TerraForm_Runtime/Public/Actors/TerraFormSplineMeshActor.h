// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/TerraFormSplineActor.h"
#include "TerraFormSplineMeshActor.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFORM_RUNTIME_API ATerraFormSplineMeshActor : public ATerraFormSplineActor
{
	GENERATED_BODY()
	
public:


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "SplineMeshActor|Meshes")
	TArray<class USplineMeshComponent*> SplineMeshes;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SplineMeshActor|Meshes")
	FName SplineMeshCollisionProfile;

	///Mapping of mesh configurations that can be selected from via the attribute system.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SplineMeshActor")
	TMap<FString, FMeshPoolEntry> MeshPool;

	//name of the attribute that drives mesh pool selection
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SplineActor|Attributes")
	FName MeshAttributeName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SplineActor|Attributes")
	bool AllowRandomOnEndPoints;

	//UPROPERTY()
	//bool MeshesDirty;

public:
	ATerraFormSplineMeshActor(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR 
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
# endif
	virtual void PostInitializeComponents() override;
	virtual void ProcessAttributesList() override;
	virtual void UpdateSpline() override;
protected:
	virtual void ReprocessSplineMeshes();
	void ProcessSplineMeshSection(int32 Start, int32 Stop, FMeshPoolEntry& MeshEntry, FVector& Scale, FRotator& DeltaRot, int32& ComponentIndex);

public:
	void OnSplineModified();

public:
	virtual void GetSupportedAttributeNames_Implementation(TArray<FName>& StringNames, TArray<FName>& FloatNames, TArray<FName>& IntegerNames, TArray<FName>& BoolNames);

	UFUNCTION(BlueprintCallable, Category = "MeshPool")
	void SetMeshPool(TMap<FString, FMeshPoolEntry> NewMeshPool);
};
