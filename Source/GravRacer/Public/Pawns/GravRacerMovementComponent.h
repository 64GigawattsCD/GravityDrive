// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Physics/PIDObject.h"
#include "Physics/GravRacerEngine.h"
//#include "WheeledVehicleMovementComponent4W.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GravRacerMovementComponent.generated.h"


UCLASS()
class GRAVRACER_API UGravRacerMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	static const float DensityAir;

public:

	UGravRacerMovementComponent();
	virtual void TickComponent(float DeltaSecond, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void InitializeComponent() override;

public:

	UFUNCTION(BlueprintCallable)
	AGravRacerPawn* GetGravRacer() const;

	/** Engine */
	UPROPERTY(EditAnywhere, Category = "MechanicalSetup|Engine")
	FGravRacerEngineData EngineSetup;

	UPROPERTY(EditAnywhere, Category = "MechanicalSetup|Engine")
	FGravRacerTransmissionData TransmissionSetup;

	/** Maximum steering (angular accel) versus forward speed (km/h) */
	UPROPERTY(EditAnywhere, Category = "MechanicalSetup|Steering")
	class UCurveFloat* SteeringCurve;

	//Three axis measurements of drag area in square meters, with drag coefficient pre-applied
	UPROPERTY(EditAnywhere, Category = "MechanicalSetup|Drag")
	FVector DragArea;

	//Three axis measurements of drag area while braking in square meters, with drag coefficient pre-applied
	UPROPERTY(EditAnywhere, Category = "MechanicalSetup|Drag")
	FVector DragAreaBraking;

	UPROPERTY(EditAnywhere, Category = SteeringSetup)
	float SteeringTiltDegrees;

	UPROPERTY(EditAnywhere, Category = ThrottleSetup)
	float ThrottleTiltDegrees;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	FPIDTuning UprightingPIDTuning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	float MaxUprightTorque;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	float UprightControlFactor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	FPIDTuning SteeringPIDTuning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	float SteeringControlFactor;

	//degrees per second
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	float MaxSteeringSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	FPIDTuning HoverPIDTuning;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	float MaxHoverForce;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Physics")
	float HoverControlFactor;

protected:

	int32 DebugIndex;
	float Braking;

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Physics")
	float HoverDistance;

	//Add to velocity directly
	void AddVelocity(FVector DeltaV, bool MpS = true);

	//Apply DeltaV as an impulse
	void AddDeltaV(FVector DeltaV, bool MpS = true);

public:
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void SetHoverDistance(float NewDistance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Server, Reliable, WithValidation, Category = "Engine")
	void SetEngineOn(bool NewEngineOn);


public:

	UPROPERTY(Transient, BlueprintReadOnly)
	UPIDObject* RollTorquePID;

	UPROPERTY(Transient, BlueprintReadOnly)
	UPIDObject* PitchTorquePID;

	//Yaw
	UPROPERTY(Transient, BlueprintReadOnly)
	UPIDObject* SteeringTorquePID;

	UPROPERTY(Transient, BlueprintReadOnly)
	UPIDObject* HoverForcePID;

protected:

	UPROPERTY(Transient)
	UGravRacerEngine* EngineSim;

public:

	void SetThrottleInput(float Value);
	void SetSteeringInput(float Value);
	
protected:

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetThrottle(float NewThrottle);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSteering(float NewSteering);

	UPROPERTY(Transient, Replicated)
	float Throttle;

	UPROPERTY(Transient, Replicated)
	float Steering;

	UPROPERTY()
	float DesiredSteeringSpeed;

	//UPROPERTY()
	//FHitResult GroundHit;

	UPROPERTY()
	FVector SurfaceNormal;

	UPROPERTY()
	float EngineRPM;

	bool IsAirborne;

	void ApplyPhysics(float DeltaSeconds);
	void PhysicsHover(float DeltaSeconds);
	void PhysicsUpright(float DeltaSeconds);
	void PhysicsSteering(float DeltaSeconds);
	void PhysicsThrottle(float DeltaSeconds);
	void ApplyFriction(float DeltaSeconds);
	void ApplyGravity(float DeltaSeconds);
	
	UPROPERTY()
	TArray<USceneComponent*> HoverPoints;

	void SetUpHover();

public:

	UFUNCTION(BlueprintCallable, Category = "Speed")
	FVector GetLocalSpaceVelocity() const;

	UFUNCTION(BlueprintCallable, Category = "Speed")
	float GetForwardSpeed() const;

	//Signed, - is left
	UFUNCTION(BlueprintCallable, Category = "Speed")
	float GetLateralSpeed() const;

	UFUNCTION(BlueprintCallable, Category = "Engine")
	float GetEngineRPM() const { return EngineSim->GetRPM(); };

	UFUNCTION(BlueprintCallable, Category = "Engine")
	float GetMaxEngineRPM() const { return EngineSetup.MaxRPM; };

	UFUNCTION(BlueprintCallable, Category = "Braking")
	float GetBraking() const { return Braking; };

	UFUNCTION(BlueprintCallable, Category = "Engine")
	int32 GetCurrentGear() const { return EngineSim->GetCurrentGear(); };

	UFUNCTION(BlueprintCallable, Category = "Braking")
		UPrimitiveComponent* GetUpdatedPrimitiveComponent() const {
		return Cast<UPrimitiveComponent>(UpdatedComponent);
	};
	
	friend UGravRacerEngine;
};
