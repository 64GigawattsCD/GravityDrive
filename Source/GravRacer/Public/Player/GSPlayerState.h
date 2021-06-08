// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "GSPlayerState.generated.h"

class UCharacterDataAsset;
class UVehicleDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FGSOnGameplayAttributeValueChangedDelegate, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelCharacter, UCharacterDataAsset*, Char, int32, Skin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelVehicle, UVehicleDataAsset*, Char, int32, Skin);

/**
 * 
 */
UCLASS()
class GRAVRACER_API AGSPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AGSPlayerState();

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UGSAttributeSetBase* GetAttributeSetBase() const;
	class UGSAttributeSetMobility* GetAttributeSetMovement() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|UI")
	void ShowAbilityConfirmPrompt(bool bShowPrompt);

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|UI")
	void ShowInteractionPrompt(float InteractionDuration);

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|UI")
	void HideInteractionPrompt();

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|UI")
	void StartInteractionTimer(float InteractionDuration);

	// Interaction interrupted, cancel and hide HUD interact timer
	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|UI")
	void StopInteractionTimer();

	/**
	* Getters for attributes from GDAttributeSetBase. Returns Current Value unless otherwise specified.
	*/

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	float GetHealthRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	float GetArmor() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	float GetMaxArmor() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	float GetArmorRegenRate() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	int32 GetXP() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	int32 GetXPBounty() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	int32 GetGold() const;

	UFUNCTION(BlueprintCallable, Category = "GASShooter|GSPlayerState|Attributes")
	int32 GetGoldBounty() const;

public:

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Character")
	void SetSelectedCharacter(UCharacterDataAsset* NewCharacter, int32 Skin = 0);

	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation, Category = "Vehicle")
	void SetSelectedVehicle(UVehicleDataAsset* NewVehicle, int32 Skin = 0);

protected:

	UPROPERTY(ReplicatedUsing = OnRep_Character)
	UCharacterDataAsset* Character;

	UPROPERTY(ReplicatedUsing = OnRep_Character)
	int32 CharacterSkin;

	UPROPERTY(ReplicatedUsing = OnRep_Vehicle)
	UVehicleDataAsset* Vehicle;

	UPROPERTY(ReplicatedUsing = OnRep_Vehicle)
	int32 VehicleSkin;

	UFUNCTION()
	virtual void OnRep_Character();

	UFUNCTION()
	virtual void OnRep_Vehicle();

public:

	UFUNCTION(BlueprintCallable, Category = "Character")
	UCharacterDataAsset* GetCharacter(int32& Skin) const { Skin = CharacterSkin; return Character; };

	UFUNCTION(BlueprintCallable, Category = "Character")
	UVehicleDataAsset* GetVehicle(int32& Skin) const { Skin = VehicleSkin; return Vehicle; };

	UPROPERTY(BlueprintAssignable)
	FOnSelCharacter OnCharacterSelected;

	UPROPERTY(BlueprintAssignable)
	FOnSelVehicle OnVehicleSelected;

protected:
	FGameplayTag DeadTag;
	FGameplayTag KnockedDownTag;

	UPROPERTY()
	class UGSAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	class UGSAttributeSetBase* AttributeSetBase;

	UPROPERTY()
	class UGSAttributeSetMobility* AttributeSetMovement;

	// Attribute changed delegate handles
	FDelegateHandle HealthChangedDelegateHandle;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Attribute changed callbacks
	virtual void HealthChanged(const FOnAttributeChangeData& Data);

public:
	int32 GetTeamNum();

	UFUNCTION()
	void SetPlacement(int32 NewPlacement) { Placement = NewPlacement; };

	UFUNCTION(BlueprintCallable)
	int32 GetPlacement() const { return Placement; };

	void SetRespawnLocation(FTransform NewRespawn) { RespawnLocation = NewRespawn; };

	UFUNCTION(BlueprintCallable)
	FTransform GetRespawnTransform() const { return RespawnLocation; };

protected:

	UPROPERTY(Transient, Replicated)
	FTransform RespawnLocation;

	int32 Placement;
	int32 Lap;
	int32 MajorPoint;
	int32 MinorPoint;

};
