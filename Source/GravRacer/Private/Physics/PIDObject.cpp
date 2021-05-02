// Fill out your copyright notice in the Description page of Project Settings.

//#include "GravRacer.h"
#include "PIDObject.h"

void UPIDObject::Update(float ProcessVariable, float DeltaSeconds, float& ControlOutput)
{
	Measured = ProcessVariable;
	float NewError = SetPoint - ProcessVariable;

	//The error itself shouldn't be greater than the total error, so it also needs clamped
	if (TuningConfiguration.ErrorClampingRange.Y != TuningConfiguration.ErrorClampingRange.X)
	{
		if (TuningConfiguration.ClampingAppliedAsUnwinding)
		{
			const float Range = TuningConfiguration.ErrorClampingRange.Y - TuningConfiguration.ErrorClampingRange.X;
			while (NewError < TuningConfiguration.ErrorClampingRange.X)
			{
				NewError += Range;
			}

			while (NewError > TuningConfiguration.ErrorClampingRange.Y)
			{
				NewError -= Range;
			}
		}
		else
		{
			NewError = FMath::Clamp(NewError, TuningConfiguration.ErrorClampingRange.X, TuningConfiguration.ErrorClampingRange.Y);
		}
	}

	//In this mode, keep track of specific error entries so they can age out
	if (TuningConfiguration.ErrorHistoryTime > 0.f)
	{
		for (int32 I = ErrorHistory.Num() - 1; I >= 0; I--)
		{
			ErrorHistory[I].Update(DeltaSeconds);
			if (ErrorHistory[I].Age > TuningConfiguration.ErrorHistoryTime)
			{
				ErrorHistory.RemoveAt(I);
			}
		}

		FPIDErrorStamp S = FPIDErrorStamp(NewError);
		ErrorHistory.Add(S);
		TotalError = GetTotalHistoryError();
	}
	else
	{
		TotalError += NewError * DeltaSeconds;
	}

	if (TuningConfiguration.ErrorClampingRange.Y != TuningConfiguration.ErrorClampingRange.X)
	{
		if (TuningConfiguration.ClampingAppliedAsUnwinding)
		{
			const float Range = TuningConfiguration.ErrorClampingRange.Y - TuningConfiguration.ErrorClampingRange.X;
			while (NewError < TuningConfiguration.ErrorClampingRange.X)
			{
				TotalError += Range;
			}

			while (NewError > TuningConfiguration.ErrorClampingRange.Y)
			{
				TotalError -= Range;
			}
		}
		else
		{
			TotalError = FMath::Clamp(TotalError, TuningConfiguration.ErrorClampingRange.X, TuningConfiguration.ErrorClampingRange.Y);
		}
	}

	PO = TuningConfiguration.ProportionalGain * NewError;
	IO = (TuningConfiguration.IntegralGain) * DeltaSeconds * TotalError;
	DO = (TuningConfiguration.DerivativeGain / DeltaSeconds) * (NewError - Error);
	Error = NewError;

	CO = PO + IO + DO;
	ControlOutput = CO;
}

void UPIDObject::Clear()
{
	TotalError = 0.f;
	Error = 0.f;
	PO = 0.f;
	IO = 0.f;
	DO = 0.f;
	CO = 0.f;
}

float UPIDObject::GetTotalHistoryError() const
{
	float E = 0.f;
	float A = 0.f;
	for (int32 S = ErrorHistory.Num() - 1 ; S >= 0; S--)
	{
		E += ErrorHistory[S].Error * (ErrorHistory[S].Age - A);
		A = ErrorHistory[S].Age;
	}
	return E;
}

void UPIDObject::GetDebugInfo(float& Set, float& Meas, float& Err, float& SumErr, float& P, float& I, float& D, float& C)
{
	Set = SetPoint;
	Meas = Measured;
	Err = Error;
	SumErr = TotalError;
	P = PO;
	I = IO;
	D = DO;
	C = CO;
}
