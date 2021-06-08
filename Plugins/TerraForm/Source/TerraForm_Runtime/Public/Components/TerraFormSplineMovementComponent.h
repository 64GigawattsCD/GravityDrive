// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "TerraFormSplineActor.h"
#include "TerraFormSplineMovementComponent.generated.h"

//Basic movement component for traversing along splines, taking spline connections into account when using TerraFormSplineActors

class USplineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSplineConnection, class ATFSplineConnectionActor*, Connection);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndOfSpline, ESplineDirection, Direction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSplineMovement);

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TERRAFORM_RUNTIME_API UTerraFormSplineMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

public:

	//Pointer to spline component we are currently moving along
	UPROPERTY(ReplicatedUsing = OnRep_MovementSpline, BlueprintReadWrite, EditAnywhere, Category = "SplineMovement")
	USplineComponent* MovementSpline;
	
	//Current distance along spline
	UPROPERTY(ReplicatedUsing = OnRep_SplineDistance, BlueprintReadWrite, EditAnywhere, Category = "SplineMovement")
	float SplineDistance;

	//Speed of movement along spline in UU/Sec
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "SplineMovement")
	float Speed;

	UPROPERTY(ReplicatedUsing = OnRep_SplineOffset, BlueprintReadWrite, EditAnywhere, Category = "SplineMovement")
	FVector SplineOffset;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SplineMovement")
	bool MoveSweeps;

	UPROPERTY(BlueprintAssignable, Category = "SplineMovement")
	FOnSplineConnection OnConnectionTaken;

	UPROPERTY(BlueprintAssignable, Category = "SplineMovement")
	FOnEndOfSpline OnEndOfSplineReached;
	
	//Event when the movement component moves the owning actor
	UPROPERTY(BlueprintAssignable, Category = "SplineMovement")
	FOnSplineMovement OnMovement;

public:

	UTerraFormSplineMovementComponent(const FObjectInitializer & ObjectInitializer);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction);

	//Move along the spline with a given delta in UU (cm)
	UFUNCTION(BlueprintCallable, Category = "SplineMovement")
	virtual void MoveAlongSpline(float Delta);

	//Overridable event for handling spline connnections
	UFUNCTION(BlueprintNativeEvent, Category = "SplineMovement")
	bool ProcessSplineConnection(class ATFSplineConnectionActor* InConnection, float & InDelta, float Start);
	virtual bool ProcessSplineConnection_Implementation(class ATFSplineConnectionActor* InConnection, float & InDelta, float Start);
	
	//Overridable event for calculating the transform base on the spine component and distance along the spline
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "SplineMovement")
	FTransform GetTransformOnSpline(USplineComponent * SplineComp, float Distance, FVector Offset);
	virtual FTransform GetTransformOnSpline_Implementation(USplineComponent* SplineComp, float Distance, FVector Offset);

	//Place the ownign actor directly onto the spline component without sweeping
	UFUNCTION(BlueprintCallable, Category = "SplineMovement")
	virtual void PlaceOnSpline(USplineComponent* SplineComp, float DistanceAlongSpline);

protected:

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_PlaceOnSpline(USplineComponent* SplineComp, float DistanceAlongSpline);

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_OnConnectionTaken(class ATFSplineConnectionActor* ConnectionTaken);

	UFUNCTION(Client, Reliable, WithValidation)
	void Client_OnEndOfSplineReached(ESplineDirection Direction);

	UFUNCTION()
	void OnRep_MovementSpline();

	UFUNCTION()
	void OnRep_SplineDistance();

	UFUNCTION()
	void OnRep_SplineOffset();

	void Client_UpdateMovement();

protected:

	//Used to prevent rapidly flipping back and forth across connections that flip the traversal direction
	UPROPERTY(Transient)
	class ATFSplineConnectionActor* LastConnectionTaken;
};
