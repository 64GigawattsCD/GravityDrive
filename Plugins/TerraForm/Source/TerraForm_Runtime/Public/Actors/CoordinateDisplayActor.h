// Copyright Craig Delancy 2020

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoordinateDisplayActor.generated.h"

class UTextRenderComponent;
class UBillboardComponent;
class ATerraFormProjectionAnchor;

UCLASS()
class TERRAFORM_RUNTIME_API ACoordinateDisplayActor : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coordinate")
	UBillboardComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Coordinate")
	UTextRenderComponent* CoordinateDisplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coordinate")
	ATerraFormProjectionAnchor* CoordinateReference;

protected:

	FText CoordinateText;
	
public:	
	// Sets default values for this actor's properties
	ACoordinateDisplayActor(const FObjectInitializer& ObjectInitializer);

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
public:

	void UpdateDisplayText();

	//Get the Geographic coordinate of this point
	UFUNCTION(BlueprintCallable, Category = "GIS")
	virtual FVector2D GetGeographicCoordinate(float & OutElevation);

	//Get the projected coordinate of this point
	UFUNCTION(BlueprintCallable, Category = "GIS")
	virtual FVector2D GetProjectedCoordinate(float & OutElevation);
};
