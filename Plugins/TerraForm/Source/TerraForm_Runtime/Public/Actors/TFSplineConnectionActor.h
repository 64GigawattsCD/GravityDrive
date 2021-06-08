// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TFSplineConnectionActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConnectionActiveChanged, bool, NewActive);

UCLASS(Blueprintable)
class TERRAFORM_RUNTIME_API ATFSplineConnectionActor : public AActor
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SplineConnection")
	class UBillboardComponent* Billboard;

	//A side connection point
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SplineConnection")
	class UTFSnappingSplineComponent* SplineA;

	//A side point index
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineConnection")
	int32 PointA;

	// B Side connection point
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SplineConnection")
	class UTFSnappingSplineComponent* SplineB;

	// B side point index
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineConnection")
	int32 PointB;

	//Direction the connection can be entered from via the A side
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineConnection")
	ESplineDirection ConnectionDirectionA;

	//Direction the connection can be entered from via the B side
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineConnection")
	ESplineDirection ConnectionDirectionB;

	//If true direction flips upon traversal
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SplineConnection")
	bool FlipDirection;

	//True if the  connection is active and can be taken
	UPROPERTY(ReplicatedUsing=OnRep_ConnectionActive,VisibleAnywhere, BlueprintReadOnly, Category = "SplineConnection")
	bool ConnectionActive;

	UPROPERTY(BlueprintAssignable, Category = "SplineConnection")
	FOnConnectionActiveChanged OnConnectionActiveChanged;

public:
	// Sets default values for this actor's properties
	ATFSplineConnectionActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Initialize the connection with data about each spline
	UFUNCTION(BlueprintCallable, Category = "SplineConnection")
	void SetConnectionData(class UTFSnappingSplineComponent* SA, int32 PA, class UTFSnappingSplineComponent* SB, int32 PB, ESplineDirection DA, ESplineDirection DB, bool Flip);

	UFUNCTION(BlueprintCallable, Category = "SplineConnection")
	void SetConnectionActive(bool NewConnectionActive);

	UFUNCTION(BlueprintCallable, Category = "SplineConnection")
	float GetDistanceA() const;

	UFUNCTION(BlueprintCallable, Category = "SplineConnection")
	float GetDistanceB() const;

	UFUNCTION(BlueprintCallable, Category = "SplineConnection")
	void SetSplineA(class UTFSnappingSplineComponent* SA);

	UFUNCTION(BlueprintCallable, Category = "SplineConnection")
	void SetSplineB(class UTFSnappingSplineComponent* SB);

#if WITH_EDITOR  
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChanged) override;
#endif

protected:
	UFUNCTION(Server, Reliable, WithValidation, Category = "SplineConnection")
	void ServerSetConnectionActive(bool NewConnectionActive);

	UFUNCTION()
	virtual void OnRep_ConnectionActive();
};