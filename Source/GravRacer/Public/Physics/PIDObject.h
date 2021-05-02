// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PIDObject.generated.h"

USTRUCT(BlueprintType)
struct GRAVRACER_API FPIDTuning
{
	GENERATED_BODY()

	//Constant multiplied with Error to create proportional response
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PID_Tuning")
	float ProportionalGain;

	//One divided by time in seconds for integral response to reach the proportional response
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PID_Tuning")
	float IntegralGain;

	//Seconds into the future to predict Derivative Response, using data from the same number of seconds into the past
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PID_Tuning")
	float DerivativeGain;

	//If zero, error history is tabulated indefinitely
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PID_Config")
	float ErrorHistoryTime;

	//If X != Y, applied as clamping min max
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PID_Config")
	FVector2D ErrorClampingRange;

	//If true, use unwinding alg instead of clamping
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PID_Config")
	bool ClampingAppliedAsUnwinding;

	FPIDTuning()
	{
		ErrorHistoryTime = 0.f;
		ErrorClampingRange = FVector2D::ZeroVector;
		ClampingAppliedAsUnwinding = false;
	}

	FPIDTuning(float P, float I, float D)
	{
		ProportionalGain = P;
		IntegralGain = I;
		DerivativeGain = D;
		ErrorHistoryTime = 0.f;
		ErrorClampingRange = FVector2D::ZeroVector;
		ClampingAppliedAsUnwinding = false;
	}
};


USTRUCT(BlueprintType)
struct GRAVRACER_API FPIDErrorStamp
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PID_Error")
	float Age;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PID_Error")
	float Error;

	FPIDErrorStamp()
	{
		Age = 0.f;
		Error = 0.f;
	}

	FPIDErrorStamp(float Error)
		:Error(Error)
	{
		Age = 0.f;
	}

	void Update(float DeltaSeconds)
	{
		Age += DeltaSeconds;
	}
};

UCLASS()
class GRAVRACER_API UPIDObject : public UObject
{
	GENERATED_BODY()
public:

	//Tuning settings
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PID_Tuning")
	FPIDTuning TuningConfiguration;

	//The desired value to achieve
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PID_Control")
	float SetPoint;

	UFUNCTION(BlueprintCallable)
	void Update(float ProcessVariable, float DeltaSeconds, float & ControlOutput);

	UFUNCTION(BlueprintCallable)
	void Clear();

	float GetTotalHistoryError() const;

protected:

	TArray<FPIDErrorStamp> ErrorHistory;

	float Measured;
	float Error;
	float TotalError;
	float PO;
	float IO;
	float DO;
	float CO;

public:

	UFUNCTION(BlueprintCallable, Category = "PIDdebug")
	void GetDebugInfo(float& Set, float& Meas, float& Err, float& SumErr, float& P, float& I, float& D, float& C);
	
};
