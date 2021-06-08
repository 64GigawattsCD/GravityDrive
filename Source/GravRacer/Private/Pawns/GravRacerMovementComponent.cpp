// Fill out your copyright notice in the Description page of Project Settings.

#include "GravRacerMovementComponent.h"
//#include "GravRacer.h"
#include "Pawns/GravRacerPawn.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/PhysicsVolume.h"
#include "Net/UnrealNetwork.h"

const float UGravRacerMovementComponent::DensityAir = 1.225f;

UGravRacerMovementComponent::UGravRacerMovementComponent()
{
	HoverPIDTuning = FPIDTuning(0.6f, 2.f, 0.01f);
	HoverControlFactor = 10.f;
	MaxHoverForce = 2000.f;

	bTickInEditor = false;
	MaxSteeringSpeed = 60.f;

	IsAirborne = true;
	SurfaceNormal = FVector(0.f, 0.f, 1.f);
}

void UGravRacerMovementComponent::TickComponent(float DeltaSeconds, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	DebugIndex = 0;
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

	if (GetGravRacer() && (GetGravRacer()->IsLocallyControlled() || GetGravRacer()->HasAuthority()))
	{
		ApplyPhysics(DeltaSeconds);
	}
}

void UGravRacerMovementComponent::InitializeComponent()
{
	RollTorquePID = NewObject<UPIDObject>(UPIDObject::StaticClass());
	PitchTorquePID = NewObject<UPIDObject>(UPIDObject::StaticClass());
	SteeringTorquePID = NewObject<UPIDObject>(UPIDObject::StaticClass());
	HoverForcePID = NewObject<UPIDObject>(UPIDObject::StaticClass());

	HoverForcePID->TuningConfiguration = HoverPIDTuning;
	SteeringTorquePID->TuningConfiguration = SteeringPIDTuning;
	RollTorquePID->TuningConfiguration = UprightingPIDTuning;
	PitchTorquePID->TuningConfiguration = UprightingPIDTuning;

	EngineSim = NewObject<UGravRacerEngine>(UGravRacerEngine::StaticClass());
	EngineSim->InitEngine(this);
	EngineSim->ConfigureEngine(EngineSetup, TransmissionSetup);
	EngineSim->SetEngineRunning(false);

	if (GetGravRacer())
	{
		SetUpdatedComponent(GetGravRacer()->GetMesh());

		SetUpHover();
	}
}

AGravRacerPawn* UGravRacerMovementComponent::GetGravRacer() const
{
	return Cast<AGravRacerPawn>(GetOwner());
}

void UGravRacerMovementComponent::SetHoverDistance_Implementation(float NewDistance)
{
	SetHoverDistance(NewDistance);
}

bool UGravRacerMovementComponent::SetHoverDistance_Validate(float NewDistance)
{
	return true;
}

void UGravRacerMovementComponent::SetEngineOn_Implementation(bool NewEngineOn)
{
	EngineSim->SetEngineRunning(NewEngineOn);
}

bool UGravRacerMovementComponent::SetEngineOn_Validate(bool NewEngineOn)
{
	return true;
}

void UGravRacerMovementComponent::AddVelocity(FVector DeltaV, bool MpS)
{
	if (!Cast<UPrimitiveComponent>(UpdatedComponent) || DeltaV.ContainsNaN())
	{
		return;
	}

	FVector V = Cast<UPrimitiveComponent>(UpdatedComponent)->GetPhysicsLinearVelocity();
	Cast<UPrimitiveComponent>(UpdatedComponent)->SetPhysicsLinearVelocity(V + (MpS?DeltaV * 100.f :DeltaV));
}

void UGravRacerMovementComponent::AddDeltaV(FVector DeltaV, bool MpS /*= true*/)
{
	if (!Cast<UPrimitiveComponent>(UpdatedComponent) || DeltaV.ContainsNaN())
	{
		return;
	}

	Cast<UPrimitiveComponent>(UpdatedComponent)->AddImpulse(MpS ? DeltaV * 100.f : DeltaV, NAME_None, true);
}

void UGravRacerMovementComponent::SetThrottleInput(float Value)
{
	Throttle = FMath::Clamp(Value, -1.f, 1.f);

	if (!GetOwner()->HasAuthority())
	{
		ServerSetThrottle(Throttle);
	}
}

void UGravRacerMovementComponent::SetSteeringInput(float Value)
{
	Steering = FMath::Clamp(Value, -1.f, 1.f);

	if (!GetOwner()->HasAuthority())
	{
		ServerSetSteering(Steering);
	}
}

void UGravRacerMovementComponent::ServerSetThrottle_Implementation(float NewThrottle)
{
	SetThrottleInput(NewThrottle);
}

bool UGravRacerMovementComponent::ServerSetThrottle_Validate(float NewThrottle)
{
	return true;
}

void UGravRacerMovementComponent::ServerSetSteering_Implementation(float NewSteering)
{
	SetSteeringInput(NewSteering);
}

bool UGravRacerMovementComponent::ServerSetSteering_Validate(float NewSteering)
{
	return true;
}

void UGravRacerMovementComponent::ApplyPhysics(float DeltaSeconds)
{
	//First friction
	ApplyFriction(DeltaSeconds);
	
	//Gravity
	//ApplyGravity(DeltaSeconds);

	//Apply control physics
	PhysicsHover(DeltaSeconds);
	PhysicsUpright(DeltaSeconds);
	PhysicsSteering(DeltaSeconds);
	PhysicsThrottle(DeltaSeconds);
}

void UGravRacerMovementComponent::SetUpHover()
{
	if (!UpdatedComponent)
	{
		return;
	}

	HoverPoints.Empty();
	TArray<USceneComponent*> Children;
	UpdatedComponent->GetChildrenComponents(false, Children);
	for (USceneComponent* Child : Children)
	{
		if (Child->ComponentHasTag("Hover"))
		{
			HoverPoints.Add(Child);
		}
	}
}

void UGravRacerMovementComponent::PhysicsHover(float DeltaSeconds)
{
	if (HoverPoints.Num() < 1)
	{
		IsAirborne = true;
		return;
	}

	float Desired = HoverDistance;
	float Dist = Desired * 1.5f;

	TArray<FHitResult> Hits;
	for (USceneComponent* Comp : HoverPoints)
	{
		FVector Start = Comp->GetComponentLocation();
		//TODO: Change this?
		FVector End = Start + FVector(0.f, 0.f, -Dist);
		FCollisionShape Shape = FCollisionShape::MakeSphere(50.f);
		FCollisionQueryParams Params = FCollisionQueryParams();
		Params.AddIgnoredActor(GetOwner());
		FCollisionResponseParams ResponseParam = FCollisionResponseParams();
		FHitResult Hit = FHitResult();
		GetWorld()->SweepSingleByChannel(Hit, Start, End, FQuat(), ECollisionChannel::ECC_Pawn, Shape, Params, ResponseParam);
		if (Hit.bBlockingHit)
		{
			Hits.Add(Hit);
		}
	}

	if (Hits.Num() < 1)
	{
		IsAirborne = true;
		SurfaceNormal = UpdatedComponent->GetUpVector();
		HoverForcePID->Clear();
	}
	else
	{
		IsAirborne = false;

		float Min = Dist;
		float Max = 0.f;
		float Mean = 0.f;
		FVector HitMin = FVector::ZeroVector;
		FVector HitMax = FVector::ZeroVector;
		FVector HitMean = FVector::ZeroVector;
		FVector Normal = FVector::ZeroVector;
		for (FHitResult& Hit : Hits)
		{
			const FVector P = UpdatedComponent->GetComponentQuat().UnrotateVector(Hit.ImpactPoint - UpdatedComponent->GetComponentLocation());

			//const float D = (Hit.ImpactPoint - Hit.TraceStart).Size();
			const float D = -P.Z;
			

			if (D > Max)
			{
				Max = D;
				HitMax = Hit.ImpactPoint;
			}

			if (D < Min)
			{
				Min = D;
				HitMin = Hit.ImpactPoint;
			}

			Mean += D;
			HitMean += Hit.ImpactPoint;
			Normal += Hit.ImpactNormal;
		}

		Mean /= float(Hits.Num());
		HitMean *= (1.f/float(Hits.Num()));
		SurfaceNormal = (Normal * (1.f / float(Hits.Num()))).GetSafeNormal();
		
		/*
		if (Hits.Num() > 3 && !HitMin.ContainsNaN() && !HitMin.ContainsNaN() && !HitMean.ContainsNaN())
		{
			const FVector A = (HitMean - HitMin).GetSafeNormal();
			const FVector B = (HitMean - HitMax).GetSafeNormal();
			const FVector Norm = FVector::CrossProduct(A, B).GetSafeNormal();
			SurfaceNormal = (SurfaceNormal.IsNearlyZero() || FVector::DotProduct(SurfaceNormal, Norm) > 0.f) ? Norm : -Norm;
		}*/
	
		//const float Measured = FMath::Clamp(Mean, FMath::Min(Min + 60.f, Mean), FMath::Max(Max - 60.f, Mean));

		//Minimum bias
		//const float Measured = FMath::Lerp(Measured, Min, 0.2f);

		float HoverForce = 0.f;
		float HoverControl = 0.f;
		HoverForcePID->SetPoint = Desired;
		HoverForcePID->Update(Mean, DeltaSeconds, HoverControl);
		HoverForce = FMath::Clamp(HoverControl * HoverControlFactor * MaxHoverForce, -MaxHoverForce, MaxHoverForce);
		const FVector DeltaV = (SurfaceNormal * HoverForce * DeltaSeconds);
		AddVelocity(DeltaV);
	}

}

void UGravRacerMovementComponent::PhysicsUpright(float DeltaSeconds)
{
	if (!IsAirborne)
	{
		FVector To = SurfaceNormal;
		FVector From = UpdatedComponent->GetUpVector();

		//Pitch
		const FVector PitchAxis = UpdatedComponent->GetRightVector();
		FVector PitchTo = FVector::VectorPlaneProject(To, PitchAxis).GetSafeNormal();
		FVector PitchFrom = FVector::VectorPlaneProject(From, PitchAxis).GetSafeNormal();
		float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(PitchTo, PitchFrom)));
		FVector Axis = FVector::CrossProduct(PitchTo, PitchFrom);

		bool Pos = (FVector::DotProduct(PitchAxis, Axis) * FMath::Sign(AngleDeg)) >= 0.f;
		AngleDeg = Pos? FMath::Abs(AngleDeg) : -(FMath::Abs(AngleDeg));
		
		PitchTorquePID->SetPoint = (Throttle * ThrottleTiltDegrees);
		float UprightControl = 0.f;
		PitchTorquePID->Update(AngleDeg, DeltaSeconds, UprightControl);
		UprightControl *= UprightControlFactor;

		float UprightTorque = FMath::Clamp(UprightControl * MaxUprightTorque, -MaxUprightTorque, MaxUprightTorque);
		Cast<UPrimitiveComponent>(UpdatedComponent)->AddAngularImpulseInDegrees(PitchAxis * UprightTorque * DeltaSeconds, NAME_None, true);
		//Cast<UPrimitiveComponent>(UpdatedComponent)->AddTorqueInDegrees(PitchAxis * UprightTorque * DeltaSeconds, NAME_None, true);

		//Roll
		const FVector RollAxis = UpdatedComponent->GetForwardVector();
		FVector RollTo = FVector::VectorPlaneProject(To, RollAxis).GetSafeNormal();
		FVector RollFrom = FVector::VectorPlaneProject(From, RollAxis).GetSafeNormal();
		AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(RollTo, RollFrom)));
		Axis = FVector::CrossProduct(RollTo, RollFrom);

		Pos = (FVector::DotProduct(RollAxis, Axis) * FMath::Sign(AngleDeg)) >= 0.f;
		AngleDeg = Pos ? FMath::Abs(AngleDeg) : -(FMath::Abs(AngleDeg));

		RollTorquePID->SetPoint = (-Steering * SteeringTiltDegrees);
		UprightControl = 0.f;
		RollTorquePID->Update(AngleDeg, DeltaSeconds, UprightControl);
		UprightControl *= UprightControlFactor;

		UprightTorque = FMath::Clamp(UprightControl * MaxUprightTorque, -MaxUprightTorque, MaxUprightTorque);
		Cast<UPrimitiveComponent>(UpdatedComponent)->AddAngularImpulseInDegrees(RollAxis * UprightTorque * DeltaSeconds, NAME_None, true);
		//Cast<UPrimitiveComponent>(UpdatedComponent)->AddTorqueInDegrees(RollAxis * UprightTorque * DeltaSeconds, NAME_None, true);
	}
	else
	{
		PitchTorquePID->Clear();
		RollTorquePID->Clear();
	}

}

void UGravRacerMovementComponent::PhysicsSteering(float DeltaSeconds)
{
	float KmH = FMath::Abs(GetForwardSpeed() * 0.036f);
	float MaxSteeringTorque = 0.f;
	if (SteeringCurve)
	{
		MaxSteeringTorque = SteeringCurve->GetFloatValue(KmH);
	}

	//Deg/Sec
	DesiredSteeringSpeed = Steering * MaxSteeringSpeed;

	FVector SteeringAxis = UpdatedComponent->GetUpVector();
	FVector AngVel = Cast<UPrimitiveComponent>(UpdatedComponent)->GetPhysicsAngularVelocityInDegrees();

	bool Pos = FVector::DotProduct(SteeringAxis, AngVel) > 0.f;

	AngVel.ProjectOnToNormal(Pos? SteeringAxis : -SteeringAxis);
	float CurrentSteeringSpeed = Pos ? AngVel.Size() : -(AngVel.Size());

	float SteeringControl = 0.f;
	SteeringTorquePID->SetPoint = DesiredSteeringSpeed;
	SteeringTorquePID->Update(CurrentSteeringSpeed, DeltaSeconds, SteeringControl);

	float SteeringTorque = FMath::Clamp(SteeringControl * SteeringControlFactor * MaxSteeringTorque, -MaxSteeringTorque, MaxSteeringTorque);
	//Cast<UPrimitiveComponent>(UpdatedComponent)->AddTorqueInDegrees(SteeringAxis * SteeringTorque * DeltaSeconds, NAME_None, true);
	Cast<UPrimitiveComponent>(UpdatedComponent)->AddAngularImpulseInDegrees(SteeringAxis * SteeringTorque * DeltaSeconds, NAME_None, true);
}

void UGravRacerMovementComponent::PhysicsThrottle(float DeltaSeconds)
{
	if (Throttle > 0.f && !EngineSim->GetEngineRunning())
	{
		//Start engine
		EngineSim->SetEngineRunning(true);
	}

	const FVector V = GetUpdatedPrimitiveComponent()->GetPhysicsLinearVelocity();
	float Thrust = 0.f;
	EngineSim->SetThrottle(Throttle);
	EngineSim->SimulateEngine(DeltaSeconds, Thrust);
	//GetUpdatedPrimitiveComponent()->AddForce(UpdatedComponent->GetForwardVector() * Thrust * 10000.f, NAME_None, false);

	//FString Message = "Thrust: " + FString::SanitizeFloat(Thrust) + "   Forward Speed: " + FString::SanitizeFloat(GetForwardSpeed());
	//GEngine->AddOnScreenDebugMessage(DebugIndex++, 0.f, FColor::Turquoise, *Message);
	//GEngine->AddOnScreenDebugMessage(DebugIndex++, 0.f, FColor::Green, *EngineSim->GetEngineState().ToString());

}

void UGravRacerMovementComponent::ApplyFriction(float DeltaSeconds)
{
	Braking = FMath::Abs(FMath::Clamp(Throttle, -1.f, 0.f));
	// ForceFric = (1/2)(pu^2)(DragCo * SurfArea)

	//Density of Air in kg/m^3
	float FluidDensity = DensityAir;
	if (GetPhysicsVolume())
	{
		FluidDensity *= GetPhysicsVolume()->FluidFriction;
	}

	FVector ForceFriction = FVector();
	//Axis looping
	for (int32 A = 0; A < 3; A++)
	{
		float V = 0.f;
		FVector Axis = FVector();
		float DArea = 0.f;
		switch (A)
		{
			case 0:
				V = GetLocalSpaceVelocity().X;
				Axis = -(FMath::Sign(V)) * UpdatedComponent->GetForwardVector();
				DArea = FMath::Lerp(DragArea.X, DragAreaBraking.X, Braking);
			break;
			case 1:
				V = GetLocalSpaceVelocity().Y;
				Axis = -(FMath::Sign(V)) * UpdatedComponent->GetRightVector();
				DArea = FMath::Lerp(DragArea.Y, DragAreaBraking.Y, Braking);
			break;
			case 2:
				V = GetLocalSpaceVelocity().Z;
				Axis = -(FMath::Sign(V))*UpdatedComponent->GetUpVector();
				DArea = FMath::Lerp(DragArea.Z, DragAreaBraking.Z, Braking);
			break;
		}

		//Velocity to meters per second
		V *= 0.01f;
		//Sum forces
		ForceFriction += ((0.5f) * FluidDensity * FMath::Pow(V, 2.f) * DArea) * Axis;
	}

	//Omit falling
	if (IsAirborne)
	{
		const FVector GravDir = FVector(0.f, 0.f, -1.f);
		ForceFriction = FVector::VectorPlaneProject(ForceFriction, GravDir);
	}

	if (!ForceFriction.ContainsNaN() && !ForceFriction.IsZero())
	{
		//Scale by time fraction and apply
		//const FVector DeltaV = ForceFriction * DeltaSeconds;
		//Cast<UPrimitiveComponent>(UpdatedComponent)->AddForce(DeltaV, NAME_None, true);

		//AddVelocity(DeltaV);
		//AddVelocity(DeltaV);

		//Cast<UPrimitiveComponent>(UpdatedComponent)->AddForce(ForceFriction, NAME_None, false);
		const float Mass = GetUpdatedPrimitiveComponent()->GetMass();
		AddDeltaV(ForceFriction * DeltaSeconds / Mass);
		FString Message = "Drag Force: " + ForceFriction.ToString();
		GEngine->AddOnScreenDebugMessage(DebugIndex++, 0.f, FColor::Turquoise, *Message);
	}
}

void UGravRacerMovementComponent::ApplyGravity(float DeltaSeconds)
{
	//TODO: custom gravity direction

	//m/s
	const FVector DeltaV = FVector(0.f, 0.f, 980.f);
	AddVelocity(DeltaV);
}

FVector UGravRacerMovementComponent::GetLocalSpaceVelocity() const
{
	return UpdatedComponent->GetComponentQuat().UnrotateVector(Cast<UPrimitiveComponent>(UpdatedComponent)->GetPhysicsLinearVelocity(NAME_None));
}

float UGravRacerMovementComponent::GetForwardSpeed() const
{
	return GetLocalSpaceVelocity().X;
}

float UGravRacerMovementComponent::GetLateralSpeed() const
{
	return GetLocalSpaceVelocity().Y;
}

void UGravRacerMovementComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGravRacerMovementComponent, HoverDistance);
	DOREPLIFETIME_CONDITION(UGravRacerMovementComponent, Throttle, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UGravRacerMovementComponent, Steering, COND_SkipOwner);
}

