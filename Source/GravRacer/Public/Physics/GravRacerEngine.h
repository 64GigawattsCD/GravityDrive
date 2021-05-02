// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GravRacerEngine.generated.h"

USTRUCT(BlueprintType)
struct FGravRacerEngineData
{
	GENERATED_BODY()

	
	/** Torque (Nm) at a given RPM*/
	UPROPERTY(EditAnywhere, Category = Setup)
	FRuntimeFloatCurve TorqueCurve;

	/** Maximum revolutions per minute of the engine */
	UPROPERTY(EditAnywhere, Category = Setup, meta = (ClampMin = "0.01", UIMin = "0.01"))
	float MaxRPM;

	/** Moment of inertia of the engine around the axis of rotation (Kgm^2). */
	UPROPERTY(EditAnywhere, Category = Setup, meta = (ClampMin = "0.01", UIMin = "0.01"))
	float MOI;

	/** Damping rate of engine when full throttle is applied (Kgm^2/s) (Change in angular momentum?)*/
	UPROPERTY(EditAnywhere, Category = Setup, AdvancedDisplay, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DampingRateFullThrottle;

	/** Damping rate of engine in at zero throttle when the clutch is engaged (Kgm^2/s)*/
	UPROPERTY(EditAnywhere, Category = Setup, AdvancedDisplay, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DampingRateZeroThrottleClutchEngaged;

	/** Damping rate of engine in at zero throttle when the clutch is disengaged (in neutral gear) (Kgm^2/s)*/
	UPROPERTY(EditAnywhere, Category = Setup, AdvancedDisplay, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DampingRateZeroThrottleClutchDisengaged;

	/** Find the peak torque produced by the TorqueCurve */
	//float FindPeakTorque() const;
};

USTRUCT(BlueprintType)
struct FGravRacerGearData
{
	GENERATED_BODY()

	/** Determines the amount RPM multiplication*/
	UPROPERTY(EditAnywhere, Category = Setup)
	float GearRatio;

	/** Value of engineRevs/maxEngineRevs that is low enough to gear down*/
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"), Category = Setup)
	float DownRatio;

	/** Value of engineRevs/maxEngineRevs that is high enough to gear up*/
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"), Category = Setup)
	float UpRatio;
};

USTRUCT(BlueprintType)
struct FGravRacerClutchData
{
	GENERATED_BODY()

	/** Inner radius of contact area in meters*/
	UPROPERTY(EditAnywhere, Category = Setup)
	float ClutchInnerRadius;

	/** Outer radius of contact area in meters*/
	UPROPERTY(EditAnywhere, Category = Setup)
	float ClutchOuterRadius;

	/** Nummber of plates in the clutch*/
	UPROPERTY(EditAnywhere, Category = Setup)
	int32 NumClutchDiscs;

	/** Max contact force of clutch plate in Newtons */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"), Category = Setup)
	float MaxClutchForce;

	/** Friction coefficient kinetic */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"), Category = Setup)
	float FrictionCoK;

	/** Friction coefficient static */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"), Category = Setup)
	float FrictionCoS;

	/** Rate that clutch opens and closes */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"), Category = Setup)
	float ClutchRate;

	float GetClutchTorque(float Closed, bool Slipping) const;
};

USTRUCT(BlueprintType)
struct FGravRacerTransmissionData
{
	GENERATED_BODY()
	/** Whether to use automatic transmission */
	UPROPERTY(EditAnywhere, Category = VehicleSetup, meta = (DisplayName = "Automatic Transmission"))
	bool bAutomatic;

	/** Time it takes to switch gears (seconds) */
	UPROPERTY(EditAnywhere, Category = Setup, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float GearSwitchTime;

	/** Minimum time it takes the automatic transmission to initiate a gear change (seconds)*/
	UPROPERTY(EditAnywhere, Category = Setup, meta = (editcondition = "bUseGearAutoBox", ClampMin = "0.0", UIMin = "0.0"))
	float GearAutoBoxLatency;

	/** The final gear ratio multiplies the transmission gear ratios.*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Setup)
	float FinalRatio;

	/*Amount of torque not lost as heat due to friction within the transmission*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Setup)
	float TransmissionEfficiency;

	/** Radius of virtual wheel (m) used to convert rotations into acceleration*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Setup)
	float SimulatedWheelRadius;

	/** Moment of inertia of drivetrain on the other side of the clutch from the engine (Kgm^2). */
	UPROPERTY(EditAnywhere, Category = Setup, meta = (ClampMin = "0.01", UIMin = "0.01"))
	float TransmissionMOI;

	/** Forward gear ratios */
	UPROPERTY(EditAnywhere, Category = Setup, AdvancedDisplay)
	TArray<FGravRacerGearData> ForwardGears;

	/** Reverse gear ratio */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Setup)
	FGravRacerGearData ReverseGear;

	/** Value of engineRevs/maxEngineRevs that is high enough to increment gear*/
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Setup, meta = (ClampMin = "0.0", UIMin = "0.0", ClampMax = "1.0", UIMax = "1.0"))
	float NeutralGearUpRatio;

	/** Clutch configuration */
	UPROPERTY(EditAnywhere, Category = Setup, AdvancedDisplay, meta = (ClampMin = "0.0", UIMin = "0.0"))
	FGravRacerClutchData Clutch;

	FGravRacerTransmissionData()
	{
		TransmissionEfficiency = 1.f;
	}
};

USTRUCT(BlueprintType)
struct FGravRacerEngineState
{
	GENERATED_BODY()

	bool EngineRunning;
	int32 CurrentGear;
	float Throttle;
	bool ClutchSet;
	float ClutchClosed;

	float EngineRPM;
	float WheelRPM;

	FGravRacerEngineState()
	{

	}

	FString ToString();
};

UCLASS()
class GRAVRACER_API UGravRacerEngine : public UObject
{
	GENERATED_BODY()

public:

	UGravRacerEngine();
	float SubStepTime;
	
private:

	FGravRacerEngineData EngineConfig;
	FGravRacerTransmissionData TransmissionConfig;
	bool EngineRunning;
	int32 CurrentGear;
	float Throttle;
	bool ClutchSet;
	float ClutchClosed;
	bool ClutchSlipping;

	bool JustSwitchedGear;

	//Rad/s
	float EngineAngVel;
	float WheelAngVel;

	int32 PendingGear; 

public:

	int32 GetCurrentGear() const { return CurrentGear; };
	float GetRPM() const { return EngineAngVel * 9.549296596425384f; };
	bool GetEngineRunning() const { return EngineRunning; };

	void InitEngine(class UGravRacerMovementComponent* OwningComp);
	void ConfigureEngine(FGravRacerEngineData EConfig, FGravRacerTransmissionData TConfig);
	void ChangeGear(int32 NewGear);
	void SetThrottle(float NewThrottle);
	void SetEngineRunning(bool NewRunning);

	//Called to update the engine simulation, delta time in second, resistance in Newtons, current speed in ms, out thrust in Newtons
	void SimulateEngine(float DeltaSeconds, float & OutThrust);
	FGravRacerEngineState GetEngineState();

protected: 
	class UGravRacerMovementComponent* OwningComponent;
	float GetEngineTorqueAtRPM(float RPM) const;
	float GetRatioForGear(int32 Gear) const;
};
