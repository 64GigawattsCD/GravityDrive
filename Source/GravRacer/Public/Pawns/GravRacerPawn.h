// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GravRacerTypes.h"
#include "Characters/GSCharacterBase.h"
//#include "UObject/ScriptInterface.h"
#include "GameplayEffectTypes.h"
#include "GravRacerPawn.generated.h"

class AGSWeapon;
class AVehicleTrackPoint;
class UGravRacerDustType;
class UGravRacerMovementComponent;
class USpringArmComponent;
class UStaticMeshComponent;
class UCameraComponent;
class UGSFloatingStatusBarWidget;

UCLASS(Blueprintable)
class GRAVRACER_API AGravRacerPawn : public AGSCharacterBase
{
	GENERATED_UCLASS_BODY()

public:

	//AGravRacerPawn(const FObjectInitializer& ObjectInitializer);

	// Begin Actor overrides
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalForce, const FHitResult& Hit) override;
	virtual void FellOutOfWorld(const UDamageType& dmgType) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	virtual void TornOff() override;
	virtual void UnPossessed() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// End Actor overrides

	// UI
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GASShooter|UI")
	TSubclassOf<class UGSFloatingStatusBarWidget> UIFloatingStatusBarClass;

	UPROPERTY()
	class UGSFloatingStatusBarWidget* UIFloatingStatusBar;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "GASShooter|UI")
	class UWidgetComponent* UIFloatingStatusBarComponent;

	UGSFloatingStatusBarWidget* GetFloatingStatusBar();
	void InitializeFloatingStatusBar();

	// Abilty Input
	void BindASCInput();

protected:
	bool bASCInputBound;

public:

	//Stats

	//Fuel vehicles are respawned with
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "")
	float SpawnFuel;

	/** notify about touching new checkpoint */
	void OnTrackPointReached(AVehicleTrackPoint* TrackPoint);

	/** is handbrake active? */
	UFUNCTION(BlueprintCallable, Category="Game|Vehicle")
	bool IsHandbrakeActive() const;

	/** get current speed */
	float GetVehicleSpeed() const;

	/** get current RPM */
	UFUNCTION(BlueprintCallable, Category = "Game|Vehicle|Engine")
	float GetEngineRotationSpeed() const;

	/** get maximum RPM */
	UFUNCTION(BlueprintCallable, Category = "Game|Vehicle|Engine")
	float GetEngineMaxRotationSpeed() const;

	/** Get the ratio of RPM out of Max RPM */
	UFUNCTION(BlueprintCallable, Category = "Game|Vehicle|Engine")
	float GetEngineRPMRatio() const { return GetEngineRotationSpeed() / GetEngineMaxRotationSpeed(); };

	UFUNCTION(BlueprintCallable, Category = "Game|Vehicle|Engine")
	bool GetAfterBurnerActive() const;

	UFUNCTION(BlueprintCallable, Category = "Game|Vehicle|Engine")
	int32 GetCurrentGear() const;
		
	//////////////////////////////////////////////////////////////////////////
	// Input handlers

	/** event call on handbrake input */
	void OnHandbrakePressed();
	void OnHandbrakeReleased();

	void MoveForward(float Val);
	void MoveRight(float Val);

	struct FVehicleDesiredRPM
	{
		float DesiredRPM;
		float TimeStamp;
	};

	static bool GetVehicleDesiredRPM_AudioThread(const uint32 VehicleID, FVehicleDesiredRPM& OutDesiredRPM);

private:
	//Main body of the racer
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	//Movement component responsible for translating input into physical forces
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UGravRacerMovementComponent* MovementComponent;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category=Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** AudioThread authoritative cache of desired RPM keyed by owner ID for SoundNodeVehicleEngine to reference */
	static TMap<uint32, FVehicleDesiredRPM> VehicleDesiredRPMs;

public:
	UFUNCTION(BlueprintCallable, Category = Mesh)
	UStaticMeshComponent* GetMesh() const { return Mesh; };

	UFUNCTION(BlueprintCallable, Category = Movement)
	UGravRacerMovementComponent* GetVehicleMovement() const { return MovementComponent; };

protected:
	/** The minimum amount of normal force that must be applied to the chassis to spawn an Impact Effect */
	UPROPERTY(EditAnywhere, Category = Effects)
	float ImpactEffectNormalForceThreshold;


	/** engine sound */
	UPROPERTY(Category=Effects, EditDefaultsOnly)
	USoundCue* EngineSound;

private:
	/** audio component for engine sounds */
	UPROPERTY(Category = Effects, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAudioComponent* EngineAC;

private:
	/** audio component for skid sounds */
	UPROPERTY()
	UAudioComponent* SkidAC;
protected:

	/** The amount of spring compression required during landing to play sound */
	UPROPERTY(Category = Effects, EditDefaultsOnly)
	float SpringCompressionLandingThreshold;

	/** whether tires are currently touching ground */
	bool bTiresTouchingGround;

	/** if skidding is shorter than this value, SkidSoundStop won't be played */
	UPROPERTY(Category=Effects, EditDefaultsOnly)
	float SkidDurationRequiredForStopSound;

	/** is vehicle currently skidding */
	bool bSkidding;

	/** time when skidding started */
	float SkidStartTime;

	/** camera shake on impact */
	//UPROPERTY(Category=Effects, EditDefaultsOnly)
	//TSubclassOf<class UCameraShake> ImpactCameraShake;

	/** How much throttle forward (max 1.0f) or reverse (max -1.0f) */
	float ThrottleInput;

	/** How far the wheels are turned to the right (max 1.0f) or to the left (max -1.0f) */
	float TurnInput;

	/** is handbrake active? */
	uint32 bHandbrakeActive : 1;

	/** if key is being held to control the throttle, ignore other controllers */
	uint32 bKeyboardThrottle : 1;

	/** if key is being held to control the turning, ignore other controllers */
	uint32 bKeyboardTurn : 1;

	/** if turn left action key is pressed */
	uint32 bTurnLeftPressed : 1;

	/** if turn right action key is pressed */
	uint32 bTurnRightPressed : 1;

	/** if accelerate action key is pressed */
	uint32 bAcceleratePressed : 1;

	/** if break/reverse action key is pressed */
	uint32 bBreakReversePressed : 1;


	/** when entering new surface type, spawn new particle system, allowing old one to fade away nicely */
	void SpawnNewWheelEffect(int WheelIndex);

	/** update effects under wheels */
	void UpdateWheelEffects(float DeltaTime);


protected:
	/** Returns SpringArm subobject **/
	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }
	/** Returns EngineAC subobject **/
	FORCEINLINE UAudioComponent* GetEngineAC() const { return EngineAC; }
	/** Returns SkidAC subobject **/
	FORCEINLINE UAudioComponent* GetSkidAC() const { return SkidAC; }

public:
	UPROPERTY(EditAnywhere, Category = "Bots|Behavior")
	class UBehaviorTree* BotBehavior;

	virtual bool IsFirstPerson() const;

	virtual void FaceRotation(FRotator NewRotation, float DeltaTime = 0.f) override;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = "Weapons")
	AGSWeapon* CurrentWeapon;

	/** Weapons **/
	virtual AGSWeapon* GetCurrentWeapon();

	UPROPERTY()
	FName WeaponAttachPoint;

	UFUNCTION()
	FName GetWeaponAttachPoint() { return WeaponAttachPoint; };

	UFUNCTION(BlueprintCallable, Category = "GASShooter|Inventory")
	bool AddWeaponToInventory(AGSWeapon* NewWeapon);

	// Removes a weapon from the inventory.
	// Returns true if the weapon exists and was removed. False if the weapon didn't exist in the inventory.
	UFUNCTION(BlueprintCallable, Category = "GASShooter|Inventory")
	bool RemoveWeaponFromInventory(AGSWeapon* WeaponToRemove);

};



