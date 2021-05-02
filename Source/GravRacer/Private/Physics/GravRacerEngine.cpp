// Fill out your copyright notice in the Description page of Project Settings.


#include "GravRacerEngine.h"
#include "Pawns/GravRacerMovementComponent.h"

UGravRacerEngine::UGravRacerEngine()
{
	EngineRunning = false;
	CurrentGear = 0;
	Throttle = 0.f;
	ClutchSet = false;
	ClutchClosed = 0.f;
	EngineAngVel = 0.f;
	PendingGear = -2;
	JustSwitchedGear = false;
	ClutchSlipping = false;

	SubStepTime = 0.05f;
}

void UGravRacerEngine::InitEngine(class UGravRacerMovementComponent* OwningComp)
{
	OwningComponent = OwningComp;
}

void UGravRacerEngine::ConfigureEngine(FGravRacerEngineData EConfig, FGravRacerTransmissionData TConfig)
{
	EngineConfig = EConfig;
	TransmissionConfig = TConfig;
}

void UGravRacerEngine::ChangeGear(int32 NewGear)
{
	PendingGear = NewGear;
	ClutchSet = false;
}

void UGravRacerEngine::SetThrottle(float NewThrottle)
{
	Throttle = NewThrottle;
}

void UGravRacerEngine::SetEngineRunning(bool NewRunning)
{
	if (EngineRunning == NewRunning)
	{
		return;
	}
	else
	{
		EngineRunning = NewRunning;
	}
}

void UGravRacerEngine::SimulateEngine(float DeltaSeconds, float& OutThrust)
{
	//sub-stepping
	float RemainingTime = SubStepTime;

	const float Mass = OwningComponent->GetUpdatedPrimitiveComponent()->GetMass();

	while (RemainingTime > 0.f)
	{
		float DeltaTime = FMath::Min(RemainingTime, SubStepTime);
		const float RPMRatio = GetRPM() / EngineConfig.MaxRPM;
		const float Throt = (CurrentGear == 0 ? FMath::Abs(Throttle) : FMath::Clamp((CurrentGear == -1 ? Throttle * -1.f : Throttle), 0.f, 1.f));

		//Work the clutch
		if (ClutchSet && ClutchClosed < 1.f)
		{
			ClutchClosed = FMath::Clamp(ClutchClosed + (TransmissionConfig.Clutch.ClutchRate * DeltaTime), 0.f, 1.f);
		}
		else if (!ClutchSet && ClutchClosed > 0.f)
		{
			ClutchClosed = FMath::Clamp(ClutchClosed - (TransmissionConfig.Clutch.ClutchRate * DeltaTime), 0.f, 1.f);
		}

		if (ClutchClosed >= 1.f)
		{
			JustSwitchedGear = false;
		}

		//Transmission
		if (PendingGear != -2 && ClutchClosed == 0.f)
		{
			//Switch gears once clutch is open
			CurrentGear = PendingGear;
			PendingGear = -2;
			ClutchSet = CurrentGear != 0;
			JustSwitchedGear = ClutchSet;
		}
		else if (PendingGear == -2 && !JustSwitchedGear)
		{
			//Automatic transmission
			if (TransmissionConfig.bAutomatic)
			{
				//In neutral
				if (CurrentGear == 0)
				{
					if (RPMRatio > TransmissionConfig.NeutralGearUpRatio)
					{
						if (Throttle >= 0.f)
						{
							ChangeGear(1);
						}
						else
						{
							ChangeGear(-1);
						}
					}
				}
				else
				{
					FGravRacerGearData GearData = (CurrentGear == -1) ? TransmissionConfig.ReverseGear : TransmissionConfig.ForwardGears[CurrentGear - 1];
					if (RPMRatio < GearData.DownRatio)
					{
						ChangeGear((CurrentGear == -1) ? 0 : CurrentGear-1);
					}
					else if (RPMRatio > GearData.UpRatio && CurrentGear > 0 && CurrentGear < TransmissionConfig.ForwardGears.Num())
					{
						ChangeGear(CurrentGear + 1);
					}
				}
			}
		}

		//Speed in m/s
		const float Speed = OwningComponent->GetForwardSpeed() / 100.f;
		WheelAngVel = Speed / TransmissionConfig.SimulatedWheelRadius;

		const float GearRatio = GetRatioForGear(CurrentGear) * TransmissionConfig.FinalRatio;
		//Torque off the engine
		const float EngineTorque = EngineRunning ? GetEngineTorqueAtRPM(GetRPM()) * Throt : 0.f;
		//rad/s at flywheel
		EngineAngVel += (EngineTorque / EngineConfig.MOI) * DeltaTime;

		float TransmissionAngVel = WheelAngVel * GearRatio;
		float ClutchTransmissionTorque = 0.f;
	
		if (CurrentGear != 0 && ClutchClosed > 0.f)
		{
			const float ClutchTorque = TransmissionConfig.Clutch.GetClutchTorque(ClutchClosed, ClutchSlipping);
			const float MomentumEngine = EngineConfig.MOI * EngineAngVel;
			const float MomentumTransmission = TransmissionConfig.TransmissionMOI * TransmissionAngVel;
			const float DestinationAngVel = (MomentumEngine + MomentumTransmission) / (EngineConfig.MOI + TransmissionConfig.TransmissionMOI);

			const float TargetDeltaE = DestinationAngVel - EngineAngVel;
			const float DeltaE = FMath::Clamp(ClutchTorque / EngineConfig.MOI * DeltaTime * FMath::Sign(TargetDeltaE), -FMath::Abs(TargetDeltaE) , FMath::Abs(TargetDeltaE));
			EngineAngVel += DeltaE;

			const float TargetDeltaT = DestinationAngVel - TransmissionAngVel;
			const float DeltaT = FMath::Clamp(ClutchTorque / TransmissionConfig.TransmissionMOI * DeltaTime * FMath::Sign(TargetDeltaT), -FMath::Abs(TargetDeltaT), FMath::Abs(TargetDeltaT));
			TransmissionAngVel += DeltaT;
			WheelAngVel = TransmissionAngVel / GearRatio;

			ClutchSlipping = FMath::IsNearlyEqual(EngineAngVel, TransmissionAngVel);
			
			float ClutchEngineTorque = DeltaE * EngineConfig.MOI / DeltaTime;
			ClutchTransmissionTorque = DeltaT * TransmissionConfig.TransmissionMOI / DeltaTime;

			//FString Message = "Clutch Torque On Engine: " + FString::SanitizeFloat(ClutchEngineTorque) + "   Clutch Torque On Trans: " + FString::SanitizeFloat(ClutchTransmissionTorque);
			//GEngine->AddOnScreenDebugMessage(5, 0.f, FColor::Orange, *Message);
		}
		else
		{
			ClutchSlipping = true;
		}

		//Only torque that was applied via the clutch makes it through to the wheels, right?
		const float WheelTorque = ClutchTransmissionTorque * GearRatio * TransmissionConfig.TransmissionEfficiency;
		const float WheelForce = WheelTorque / TransmissionConfig.SimulatedWheelRadius;

		const float EngineDampingRate = FMath::Lerp(FMath::Lerp(EngineConfig.DampingRateZeroThrottleClutchEngaged, EngineConfig.DampingRateZeroThrottleClutchDisengaged, 1.f - ClutchClosed), EngineConfig.DampingRateFullThrottle, Throt);
		EngineAngVel -= ((EngineDampingRate * DeltaTime) / EngineConfig.MOI) * EngineAngVel;
		EngineAngVel = FMath::Clamp(GetRPM(), 0.f, EngineConfig.MaxRPM) * 0.104719755f;
	
		const float Accel = WheelForce / Mass;
		const float DeltaV = Accel * DeltaTime;
		OwningComponent->AddVelocity(DeltaV * OwningComponent->GetUpdatedPrimitiveComponent()->GetForwardVector(), true);
		OutThrust = WheelForce;
		RemainingTime -= DeltaTime;

		const float NewSpeed = OwningComponent->GetForwardSpeed() / 100.f;
		const float ActualDeltaV = NewSpeed - Speed;
		const float ActualAccel = ActualDeltaV / DeltaTime;

		FString Message = "Wheel Force: " + FString::SanitizeFloat(WheelForce);
		GEngine->AddOnScreenDebugMessage(3, 0.f, FColor::Blue, *Message);
		Message = "Applied Acceleration: " + FString::SanitizeFloat(Accel) + "   Actual Acceleration: " + FString::SanitizeFloat(ActualAccel);
		GEngine->AddOnScreenDebugMessage(4, 0.f, FColor::Turquoise, *Message);
		Message = "Expected DeltaV: " + FString::SanitizeFloat(DeltaV) + "   Actual DeltaV: " + FString::SanitizeFloat(ActualDeltaV);
		GEngine->AddOnScreenDebugMessage(5, 0.f, FColor::Green, *Message);
	}
}

FGravRacerEngineState UGravRacerEngine::GetEngineState()
{
	FGravRacerEngineState S = FGravRacerEngineState();
	S.ClutchSet = ClutchSet;
	S.ClutchClosed = ClutchClosed;
	S.EngineRPM = GetRPM();
	S.EngineRunning = EngineRunning;
	S.Throttle = Throttle;
	S.CurrentGear = CurrentGear;
	S.WheelRPM = WheelAngVel * 9.549296596425384f;
	return S;
}

float UGravRacerEngine::GetEngineTorqueAtRPM(float RPM) const
{
	return EngineConfig.TorqueCurve.GetRichCurveConst()->Eval(RPM);
}

float UGravRacerEngine::GetRatioForGear(int32 Gear) const
{
	if (Gear == 0)
	{
		return 0.f;
	}
	{
		return (Gear == -1) ? TransmissionConfig.ReverseGear.GearRatio : TransmissionConfig.ForwardGears[Gear - 1].GearRatio;
	}
}

FString FGravRacerEngineState::ToString()
{
	FString S = FString();
	S = "Clutch: " + FString::SanitizeFloat(ClutchClosed);
	S += ", ";
	S += "Throttle: " + FString::SanitizeFloat(Throttle);
	S += ", ";
	S += "Engine RPM: " + FString::SanitizeFloat(EngineRPM);
	S += ", ";
	S += "Wheel RPM: " + FString::SanitizeFloat(WheelRPM);
	S += ", ";
	S += "Gear" + FString::FromInt(CurrentGear);

	return S;
}

float FGravRacerClutchData::GetClutchTorque(float Closed, bool Slipping) const
{
	const float Q = (FMath::Pow(ClutchOuterRadius, 3.f) - FMath::Pow(ClutchInnerRadius, 3.f)) / (FMath::Pow(ClutchOuterRadius, 2.f) - FMath::Pow(ClutchInnerRadius, 2.f));
	return NumClutchDiscs * (2.f / 3.f) * (Slipping ? FrictionCoK : FrictionCoS) * Q * (MaxClutchForce * Closed);
}
