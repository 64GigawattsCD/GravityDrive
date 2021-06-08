// Copyright Craig Delancy 2020

#pragma once

#include "CoreMinimal.h"
#include "Actors/TerraFormPointActor.h"
#include "TerraFormPointMesh.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFORM_RUNTIME_API ATerraFormPointMesh : public ATerraFormPointActor
{
	GENERATED_BODY()

public:

	//Mesh component
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PointMesh|Meshes")
	class UStaticMeshComponent* Mesh;

	//Mapping of mesh configurations that can be selected from via the attribute system.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PointMesh|Meshes")
	TMap<FString, FMeshPoolEntry> MeshPool;

	//name of the attribute that drives mesh pool selection
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "PointActor|Attributes")
	FName MeshAttributeName;

	//Constructor
	ATerraFormPointMesh(const FObjectInitializer& ObjectInitializer);

	//Function for setting the static mesh on this actor
	UFUNCTION(BlueprintCallable, Category = "PointMesh")
	void SetStaticMesh(UStaticMesh* NewMesh);

	//Overriden to add the mesh attribute
	virtual void GetSupportedAttributeNames_Implementation(TArray<FName>& StringNames, TArray<FName>& FloatNames, TArray<FName>& IntegerNames, TArray<FName>& BoolNames) override;

	UFUNCTION(BlueprintCallable, Category = "MeshPool")
	void SetMeshPool(TMap<FString, FMeshPoolEntry> NewMeshPool);

	virtual void UpdateFromMeshPool();
	virtual void ApplyMeshPoolEntry(FMeshPoolEntry& Entry);
	virtual void ProcessAttributesList() override;
};
