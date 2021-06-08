// Copyright Craig Delancy 2020


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerraFormShapeDataActor.generated.h"

//Class when placed in the world, creates actors per each layer in the GIS shape asset

#if TFCONFIG > 1
class UGISShapeAsset;
#endif

class UBillboardComponent;
class ATerraFormProjectionAnchor;

UCLASS(Blueprintable)
class TERRAFORM_RUNTIME_API ATerraFormShapeDataActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBillboardComponent* Root;

#if TFCONFIG > 1
	//Asset this actor pulls data from
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GISShape")
	UGISShapeAsset* GISShapeAsset;
#endif

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GISShape")
	int32 ShapeIndex;

	
public:	
	// Sets default values for this actor's properties
	ATerraFormShapeDataActor(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	ATerraFormProjectionAnchor* GetProjectionAnchor();

#if WITH_EDITOR 
	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;
#endif
	
	//Shape attributes
	UFUNCTION(BlueprintCallable, Category = "ShapeActor|Attributes")
	virtual void ProcessAttributesList();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ShapeActor|Attributes")
	void BP_ProcessAttributesList();
	
	UFUNCTION(BlueprintNativeEvent, Category = "ShapeActor|Attributes")
	void GetSupportedAttributeNames(TArray<FName>& StringNames, TArray<FName>& FloatNames, TArray<FName>& IntegerNames, TArray<FName>& BoolNames);
};