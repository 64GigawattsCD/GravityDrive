// Copyright Epic Games, Inc. All Rights Reserved.AGravRacerPawn

#include "Pawns/GravRacerPawn.h"

#include "Player/GSPlayerState.h"
#include "Player/GSPlayerController.h"
#include "AI/GravRacerAIController.h"
#include "Pawns/GravRacerMovementComponent.h"

#include "Track/VehicleTrackPoint.h"

#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShake.h"

#include "Characters/Abilities/GSAbilitySystemComponent.h"
#include "Characters/Abilities/GSAbilitySystemGlobals.h"
#include "Characters/Abilities/AttributeSets/GSAttributeSetBase.h"

#include "UI/GSFloatingStatusBarWidget.h"

#include "Weapons/GSWeapon.h"

#include "GSBlueprintFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

TMap<uint32, AGravRacerPawn::FVehicleDesiredRPM> AGravRacerPawn::VehicleDesiredRPMs;

AGravRacerPawn::AGravRacerPawn(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer)
{
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh0"));
	Mesh->SetSimulatePhysics(true);
	SetRootComponent(Mesh);
	Mesh->SetIsReplicated(true);
	Mesh->SetEnableGravity(true);
	
	MovementComponent = CreateDefaultSubobject<UGravRacerMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->SetIsReplicated(true);

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->TargetOffset = FVector(0.f, 0.f, 400.f);
	SpringArm->SetRelativeRotation( FRotator(0.f, 0.f, 0.f) );
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 675.0f; 
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 7.f;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = true;	

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	EngineAC = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineAudio"));
	EngineAC->SetupAttachment(Mesh);

	SkidAC = CreateDefaultSubobject<UAudioComponent>(TEXT("SkidAudio"));
	SkidAC->bAutoActivate = false;	//we don't want to start skid right away
	SkidAC->SetupAttachment(Mesh);
	SkidDurationRequiredForStopSound = 1.5f;
	
	SpringCompressionLandingThreshold = 250000.f;
	bTiresTouchingGround = false;

	ImpactEffectNormalForceThreshold = 100000.f;
	AIControllerClass = AGravRacerAIController::StaticClass();

	bASCInputBound = false;

	UIFloatingStatusBarComponent = CreateDefaultSubobject<UWidgetComponent>(FName("UIFloatingStatusBarComponent"));
	UIFloatingStatusBarComponent->SetupAttachment(RootComponent);
	UIFloatingStatusBarComponent->SetRelativeLocation(FVector(0, 0, 120));
	UIFloatingStatusBarComponent->SetWidgetSpace(EWidgetSpace::Screen);
	UIFloatingStatusBarComponent->SetDrawSize(FVector2D(500, 500));

	UIFloatingStatusBarClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/GASShooter/UI/UI_FloatingStatusBar_Hero.UI_FloatingStatusBar_Hero_C"));
	if (!UIFloatingStatusBarClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find UIFloatingStatusBarClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
	}

	AutoPossessAI = EAutoPossessAI::PlacedInWorld;
	AIControllerClass = AGravRacerAIController::StaticClass();

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UGSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	//TODO: revisit this if full replication needed
	// Mixed mode would mean we only are replicated the GEs to ourself, not the GEs to simulated proxies.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	//Make sure we have actor info, hookup ability system component to actor info
	FGameplayAbilityActorInfo* actorInfo = new FGameplayAbilityActorInfo();
	actorInfo->InitFromActor(this, this, AbilitySystemComponent);
	AbilitySystemComponent->AbilityActorInfo = TSharedPtr<FGameplayAbilityActorInfo>(actorInfo);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSetBase = CreateDefaultSubobject<UGSAttributeSetBase>(TEXT("AttributeSetBase"));
}

/**
* On the Server, Possession happens before BeginPlay.
* On the Client, BeginPlay happens before Possession.
* So we can't use BeginPlay to do anything with the AbilitySystemComponent because we don't have it until the PlayerState replicates from possession.
*/
void AGravRacerPawn::BeginPlay()
{
	Super::BeginPlay();

	// Only needed for Heroes placed in world and when the player is the Server.
	// On respawn, they are set up in PossessedBy.
	// When the player a client, the floating status bars are all set up in OnRep_PlayerState.
	InitializeFloatingStatusBar();

}

void AGravRacerPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	// Clear CurrentWeaponTag on the ASC. This happens naturally in UnEquipCurrentWeapon() but
	// that is only called on the server from hero death (the OnRep_CurrentWeapon() would have
	// handled it on the client but that is never called due to the hero being marked pending
	// destroy). This makes sure the client has it cleared.
	if (AbilitySystemComponent)
	{
		/*
		AbilitySystemComponent->RemoveLooseGameplayTag(CurrentWeaponTag);
		CurrentWeaponTag = NoWeaponTag;
		AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);*/
	}

	Super::EndPlay(EndPlayReason);
}

void AGravRacerPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (EngineAC)
	{
		EngineAC->SetSound(EngineSound);
		EngineAC->Play();
	}

	if (SkidAC)
	{
		//SkidAC->SetSound(SkidSound);
		//SkidAC->Stop();
	}
}

void AGravRacerPawn::BindASCInput()
{
	if (!bASCInputBound && IsValid(AbilitySystemComponent) && IsValid(InputComponent))
	{
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
			FString("CancelTarget"), FString("EGSAbilityInputID"), static_cast<int32>(EGSAbilityInputID::Confirm), static_cast<int32>(EGSAbilityInputID::Cancel)));

		bASCInputBound = true;
	}
}


void AGravRacerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AGravRacerPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGravRacerPawn::MoveRight);

	PlayerInputComponent->BindAction("Handbrake", IE_Pressed, this, &AGravRacerPawn::OnHandbrakePressed);
	PlayerInputComponent->BindAction("Handbrake", IE_Released, this, &AGravRacerPawn::OnHandbrakeReleased);
}

void AGravRacerPawn::MoveForward(float Val)
{
	//AGravRacerPlayerController* MyPC = Cast<AGravRacerPlayerController>(GetController());

	GetVehicleMovement()->SetThrottleInput(Val);
}

void AGravRacerPawn::MoveRight(float Val)
{
	//AGravRacerPlayerController* MyPC = Cast<AGravRacerPlayerController>(GetController());

	GetVehicleMovement()->SetSteeringInput(Val);
}

bool AGravRacerPawn::GetAfterBurnerActive() const
{
	return false;
}

int32 AGravRacerPawn::GetCurrentGear() const
{
	return GetVehicleMovement()->GetCurrentGear();
}

void AGravRacerPawn::OnHandbrakePressed()
{
	/*
	AGravRacerPlayerController *VehicleController = Cast<AGravRacerPlayerController>(GetController());
	UWheeledVehicleMovementComponent* VehicleMovementComp = GetVehicleMovementComponent();
	if (VehicleMovementComp != nullptr)
	{
		VehicleMovementComp->SetHandbrakeInput(true);
	}*/
}

void AGravRacerPawn::OnHandbrakeReleased()
{
	/*
	bHandbrakeActive = false;
	UWheeledVehicleMovementComponent* VehicleMovementComp = GetVehicleMovementComponent();
	if (VehicleMovementComp != nullptr)
	{
		GetVehicleMovementComponent()->SetHandbrakeInput(false);
	}*/
}

void AGravRacerPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateWheelEffects(DeltaSeconds);

	if (AGSPlayerController* VehiclePC = Cast<AGSPlayerController>(GetController()))
	{
		const uint32 VehiclePCID = VehiclePC->GetUniqueID();
		FVehicleDesiredRPM DesiredRPM;
		DesiredRPM.DesiredRPM = GetEngineRotationSpeed();
		DesiredRPM.TimeStamp = GetWorld()->GetTimeSeconds();
		FAudioThread::RunCommandOnAudioThread([VehiclePCID, DesiredRPM]()
		{
			VehicleDesiredRPMs.Add(VehiclePCID, DesiredRPM);
		});
	}

	if (EngineAC)
	{
		EngineAC->SetFloatParameter("RPM", GetEngineRotationSpeed());
	}
}

void AGravRacerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AGSPlayerState* PS = GetPlayerState<AGSPlayerState>();
	if (PS)
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystemComponent = Cast<UGSAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSetBase = PS->GetAttributeSetBase();
		AttributeSetMovement = PS->GetAttributeSetMovement();

		//AmmoAttributeSet = PS->GetAmmoAttributeSet();

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();

		AddStartupEffects();

		AddCharacterAbilities();

		AGSPlayerController* PC = Cast<AGSPlayerController>(GetController());
		if (PC)
		{
			PC->CreateHUD();
		}

		if (AbilitySystemComponent->GetTagCount(DeadTag) > 0)
		{
			// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
			SetHealth(GetMaxHealth());
			SetFuel(SpawnFuel);
		}

		// Remove Dead tag
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(DeadTag));

		InitializeFloatingStatusBar();

		// If player is host on listen server, the floating status bar would have been created for them from BeginPlay before player possession, hide it
		if (IsLocallyControlled() && IsPlayerControlled() && UIFloatingStatusBarComponent && UIFloatingStatusBar)
		{
			UIFloatingStatusBarComponent->SetVisibility(false, true);
		}
	}

	//SetupStartupPerspective();
}

UGSFloatingStatusBarWidget* AGravRacerPawn::GetFloatingStatusBar()
{
	return UIFloatingStatusBar;
}

void AGravRacerPawn::InitializeFloatingStatusBar()
{
	// Only create once
	if (UIFloatingStatusBar || !IsValid(AbilitySystemComponent))
	{
		return;
	}

	// Don't create for locally controlled player. We could add a game setting to toggle this later.
	if (IsPlayerControlled() && IsLocallyControlled())
	{
		return;
	}

	// Need a valid PlayerState
	if (!GetPlayerState())
	{
		return;
	}

	// Setup UI for Locally Owned Players only, not AI or the server's copy of the PlayerControllers
	AGSPlayerController* PC = Cast<AGSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC && PC->IsLocalPlayerController())
	{
		if (UIFloatingStatusBarClass)
		{
			UIFloatingStatusBar = CreateWidget<UGSFloatingStatusBarWidget>(PC, UIFloatingStatusBarClass);
			if (UIFloatingStatusBar && UIFloatingStatusBarComponent)
			{
				UIFloatingStatusBarComponent->SetWidget(UIFloatingStatusBar);

				// Setup the floating status bar
				UIFloatingStatusBar->SetHealthPercentage(GetHealth() / GetMaxHealth());
				//UIFloatingStatusBar->SetManaPercentage(GetMana() / GetMaxMana());
				//UIFloatingStatusBar->SetShieldPercentage(GetShield() / GetMaxShield());
				UIFloatingStatusBar->OwningCharacter = this;
				UIFloatingStatusBar->SetCharacterName(CharacterName);
			}
		}
	}
}

void AGravRacerPawn::UnPossessed()
{
	if (AGSPlayerController* VehiclePC = Cast<AGSPlayerController>(GetController()))
	{
		const uint32 VehiclePCID = VehiclePC->GetUniqueID();
		FAudioThread::RunCommandOnAudioThread([VehiclePCID]()
		{
			VehicleDesiredRPMs.Remove(VehiclePCID);
		});
	}

	// Super clears controller, so do the behavior first
	Super::UnPossessed();
}

void AGravRacerPawn::OnRep_Controller()
{
	Super::OnRep_Controller();

	// Our controller changed, must update ActorInfo on AbilitySystemComponent
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RefreshAbilityActorInfo();
	}
}

bool AGravRacerPawn::GetVehicleDesiredRPM_AudioThread(const uint32 VehicleID, FVehicleDesiredRPM& OutDesiredRPM)
{
	check(IsInAudioThread());
	if (FVehicleDesiredRPM* DesiredRPM = VehicleDesiredRPMs.Find(VehicleID))
	{
		OutDesiredRPM = *DesiredRPM;
		return true;
	}
	return false;
}

void AGravRacerPawn::SpawnNewWheelEffect(int WheelIndex)
{
	/*
	DustPSC[WheelIndex] = NewObject<UParticleSystemComponent>(this);
	DustPSC[WheelIndex]->bAutoActivate = true;
	DustPSC[WheelIndex]->bAutoDestroy = false;
	DustPSC[WheelIndex]->RegisterComponentWithWorld(GetWorld());
	DustPSC[WheelIndex]->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, GetVehicleMovement()->WheelSetups[WheelIndex].BoneName);*/
}

void AGravRacerPawn::UpdateWheelEffects(float DeltaTime)
{
	/*
	if (GetVehicleMovement() && bTiresTouchingGround == false && LandingSound)	//we don't update bTiresTouchingGround until later in this function, so we can use it here to determine whether we're landing
	{
		float MaxSpringForce = GetVehicleMovement()->GetMaxSpringForce();
		if (MaxSpringForce > SpringCompressionLandingThreshold)
		{
			UGameplayStatics::PlaySoundAtLocation(this, LandingSound, GetActorLocation());
		}
	}

	bTiresTouchingGround = false;

	if (DustType && !bIsDying &&
		GetVehicleMovement() && GetVehicleMovement()->Wheels.Num() > 0)
	{
		const float CurrentSpeed = GetVehicleSpeed();
		for (int32 i = 0; i < UE_ARRAY_COUNT(DustPSC); i++)
		{
			UPhysicalMaterial* ContactMat = GetVehicleMovement()->Wheels[i]->GetContactSurfaceMaterial();
			if (ContactMat != nullptr)
			{
				bTiresTouchingGround = true;
			}
			UParticleSystem* WheelFX = DustType->GetDustFX(ContactMat, CurrentSpeed);

			const bool bIsActive = DustPSC[i] != nullptr && !DustPSC[i]->bWasDeactivated && !DustPSC[i]->bWasCompleted;
			UParticleSystem* CurrentFX = DustPSC[i] != nullptr ? DustPSC[i]->Template : nullptr;
			if (WheelFX != nullptr && (CurrentFX != WheelFX || !bIsActive))
			{
				if (DustPSC[i] == nullptr || !DustPSC[i]->bWasDeactivated)
				{
					if (DustPSC[i] != nullptr)
					{
						DustPSC[i]->SetActive(false);
						DustPSC[i]->bAutoDestroy = true;
					}
					SpawnNewWheelEffect(i);
				}
				DustPSC[i]->SetTemplate(WheelFX);
				DustPSC[i]->ActivateSystem();
			}
			else if (WheelFX == nullptr && bIsActive)
			{
				DustPSC[i]->SetActive(false);
			}
		}
	}

	if (SkidAC != nullptr)
	{
		FVector Vel = GetVelocity();
		bool bVehicleStopped = Vel.SizeSquared2D() < SkidThresholdVelocity*SkidThresholdVelocity;
		bool TireSlipping = GetVehicleMovement()->CheckSlipThreshold(LongSlipSkidThreshold, LateralSlipSkidThreshold);
		bool bWantsToSkid = bTiresTouchingGround && !bVehicleStopped && TireSlipping;

		float CurrTime = GetWorld()->GetTimeSeconds();
		if (bWantsToSkid && !bSkidding)
		{
			bSkidding = true;
			SkidAC->Play();
			SkidStartTime = CurrTime;
		}
		if (!bWantsToSkid && bSkidding)
		{
			bSkidding = false;
			SkidAC->FadeOut(SkidFadeoutTime, 0);
			if (CurrTime - SkidStartTime > SkidDurationRequiredForStopSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, SkidSoundStop, GetActorLocation());
			}
		}
	}*/
}

void AGravRacerPawn::OnTrackPointReached(AVehicleTrackPoint* NewCheckpoint)
{
	AGSPlayerController* MyPC = Cast<AGSPlayerController>(GetController());
	if (MyPC)
	{
		MyPC->OnTrackPointReached(NewCheckpoint);
	}
}

bool AGravRacerPawn::IsHandbrakeActive() const
{
	return bHandbrakeActive;
}

float AGravRacerPawn::GetVehicleSpeed() const
{
	return (GetVehicleMovement()) ? FMath::Abs(GetVehicleMovement()->GetForwardSpeed()) : 0.0f;
}

float AGravRacerPawn::GetEngineRotationSpeed() const
{
	return (GetVehicleMovement()) ? FMath::Abs(GetVehicleMovement()->GetEngineRPM()) : 0.0f;
}

float AGravRacerPawn::GetEngineMaxRotationSpeed() const
{
	return (GetVehicleMovement()) ? FMath::Abs(GetVehicleMovement()->GetMaxEngineRPM()) : 1.f;
}

void AGravRacerPawn::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalForce, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalForce, Hit);

	/*
	if (ImpactTemplate && NormalForce.SizeSquared() > FMath::Square(ImpactEffectNormalForceThreshold))
	{
		FTransform const SpawnTransform(HitNormal.Rotation(), HitLocation);
		AGravRacerImpactEffect* EffectActor = GetWorld()->SpawnActorDeferred<AGravRacerImpactEffect>(ImpactTemplate, SpawnTransform);
		if (EffectActor)
		{
			float DotBetweenHitAndUpRotation = FVector::DotProduct(HitNormal, GetMesh()->GetUpVector());
			EffectActor->SurfaceHit = Hit;
			EffectActor->HitForce = NormalForce;
			//EffectActor->bWheelLand = DotBetweenHitAndUpRotation > 0.8f;
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		}
	}*/

	/*
	if (ImpactCameraShake)
	{
		AGravRacerPlayerController* PC = Cast<AGravRacerPlayerController>(Controller);
		if (PC != nullptr && PC->IsLocalController())
		{
			PC->ClientPlayCameraShake(ImpactCameraShake, 1);
		}
	}*/
}

void AGravRacerPawn::FellOutOfWorld(const class UDamageType& dmgType)
{
	//Die();
}

void AGravRacerPawn::TornOff()
{
	Super::TornOff();

	SetLifeSpan(1.0f);
}

bool AGravRacerPawn::IsFirstPerson() const
{
	//TODO: revisit later
	return false;
}

void AGravRacerPawn::FaceRotation(FRotator NewRotation, float DeltaTime /*= 0.f*/)
{
	if(Cast<APlayerController>(GetController()))
	{ 
		Super::FaceRotation(NewRotation, DeltaTime);
	}
	else
	{
		//I don't know why it needs to work like this for bots, but its how shooter game does it
		FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 8.0f);
		Super::FaceRotation(CurrentRotation, DeltaTime);
	}
}

AGSWeapon* AGravRacerPawn::GetCurrentWeapon()
{
	return CurrentWeapon;
}

bool AGravRacerPawn::AddWeaponToInventory(AGSWeapon* NewWeapon)
{
	if (CurrentWeapon)
	{
		return false;
	}

	CurrentWeapon = NewWeapon;
	CurrentWeapon->SetOwningCharacter(this);
	CurrentWeapon->AddAbilities();
	return true;
}

bool AGravRacerPawn::RemoveWeaponFromInventory(AGSWeapon* WeaponToRemove)
{
	if (WeaponToRemove && CurrentWeapon == WeaponToRemove)
	{
		WeaponToRemove->RemoveAbilities();
		WeaponToRemove->SetOwningCharacter(nullptr);
		WeaponToRemove->Destroy();
		return true;
	}

	return false;
}

void AGravRacerPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Current Wepon, inventory?
	DOREPLIFETIME(AGravRacerPawn, CurrentWeapon);
}
