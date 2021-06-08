// Copyright 2020 Dan Kestranek.


#include "Player/GSPlayerState.h"
#include "Pawns/GravRacerPawn.h"
#include "Player/GSPlayerController.h"

#include "Assets/CharacterDataAsset.h"
#include "Assets/VehicleDataAsset.h"

#include "Characters/Abilities/AttributeSets/GSAttributeSetBase.h"
#include "Characters/Abilities/AttributeSets/GSAttributeSetMobility.h"
#include "Characters/Abilities/GSAbilitySystemComponent.h"
#include "Characters/Abilities/GSAbilitySystemGlobals.h"

#include "UI/GSFloatingStatusBarWidget.h"
#include "UI/GSHUDWidget.h"

#include "Weapons/GSWeapon.h"

#include "Net/UnrealNetwork.h"

AGSPlayerState::AGSPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UGSAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSetBase = CreateDefaultSubobject<UGSAttributeSetBase>(TEXT("AttributeSetBase"));
	AttributeSetMovement = CreateDefaultSubobject<UGSAttributeSetMobility>(TEXT("AttributeSetMobility"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 100.0f;

	DeadTag = FGameplayTag::RequestGameplayTag("State.Dead");
	KnockedDownTag = FGameplayTag::RequestGameplayTag("State.KnockedDown");
}

UAbilitySystemComponent* AGSPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UGSAttributeSetBase* AGSPlayerState::GetAttributeSetBase() const
{
	return AttributeSetBase;
}


class UGSAttributeSetMobility* AGSPlayerState::GetAttributeSetMovement() const
{
	return AttributeSetMovement;
}

bool AGSPlayerState::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void AGSPlayerState::ShowAbilityConfirmPrompt(bool bShowPrompt)
{
	AGSPlayerController* PC = Cast<AGSPlayerController>(GetOwner());
	if (PC)
	{
		UGSHUDWidget* HUD = PC->GetGSHUD();
		if (HUD)
		{
			HUD->ShowAbilityConfirmPrompt(bShowPrompt);
		}
	}
}

void AGSPlayerState::ShowInteractionPrompt(float InteractionDuration)
{
	AGSPlayerController* PC = Cast<AGSPlayerController>(GetOwner());
	if (PC)
	{
		UGSHUDWidget* HUD = PC->GetGSHUD();
		if (HUD)
		{
			HUD->ShowInteractionPrompt(InteractionDuration);
		}
	}
}

void AGSPlayerState::HideInteractionPrompt()
{
	AGSPlayerController* PC = Cast<AGSPlayerController>(GetOwner());
	if (PC)
	{
		UGSHUDWidget* HUD = PC->GetGSHUD();
		if (HUD)
		{
			HUD->HideInteractionPrompt();
		}
	}
}

void AGSPlayerState::StartInteractionTimer(float InteractionDuration)
{
	AGSPlayerController* PC = Cast<AGSPlayerController>(GetOwner());
	if (PC)
	{
		UGSHUDWidget* HUD = PC->GetGSHUD();
		if (HUD)
		{
			HUD->StartInteractionTimer(InteractionDuration);
		}
	}
}

void AGSPlayerState::StopInteractionTimer()
{
	AGSPlayerController* PC = Cast<AGSPlayerController>(GetOwner());
	if (PC)
	{
		UGSHUDWidget* HUD = PC->GetGSHUD();
		if (HUD)
		{
			HUD->StopInteractionTimer();
		}
	}
}

float AGSPlayerState::GetHealth() const
{
	return AttributeSetBase->GetHealth();
}

float AGSPlayerState::GetMaxHealth() const
{
	return AttributeSetBase->GetMaxHealth();
}

float AGSPlayerState::GetHealthRegenRate() const
{
	return AttributeSetBase->GetHealthRegenRate();
}

float AGSPlayerState::GetArmor() const
{
	return AttributeSetBase->GetArmor();
}

float AGSPlayerState::GetMaxArmor() const
{
	return AttributeSetBase->GetMaxArmor();
}

float AGSPlayerState::GetArmorRegenRate() const
{
	return AttributeSetBase->GetArmorRegenRate();
}

int32 AGSPlayerState::GetCharacterLevel() const
{
	return AttributeSetBase->GetCharacterLevel();
}

int32 AGSPlayerState::GetXP() const
{
	return AttributeSetBase->GetXP();
}

int32 AGSPlayerState::GetXPBounty() const
{
	return AttributeSetBase->GetXPBounty();
}

int32 AGSPlayerState::GetGold() const
{
	return AttributeSetBase->GetGold();
}

int32 AGSPlayerState::GetGoldBounty() const
{
	return AttributeSetBase->GetGoldBounty();
}


void AGSPlayerState::OnRep_Character()
{
	OnCharacterSelected.Broadcast(Character, CharacterSkin);
}

void AGSPlayerState::OnRep_Vehicle()
{
	OnVehicleSelected.Broadcast(Vehicle, VehicleSkin);
}

void AGSPlayerState::SetSelectedVehicle_Implementation(UVehicleDataAsset* NewVehicle, int32 Skin /*= 0*/)
{
	Vehicle = NewVehicle;
	VehicleSkin = Skin;
	OnRep_Vehicle();
}

bool AGSPlayerState::SetSelectedVehicle_Validate(UVehicleDataAsset* NewVehicle, int32 Skin /*= 0*/)
{
	//TODO: validate skin has been purchased
	return true;
}

void AGSPlayerState::SetSelectedCharacter_Implementation(UCharacterDataAsset* NewCharacter, int32 Skin /*= 0*/)
{
	Character = NewCharacter;
	CharacterSkin = Skin;
	OnRep_Character();
}

bool AGSPlayerState::SetSelectedCharacter_Validate(UCharacterDataAsset* NewCharacter, int32 Skin /*= 0*/)
{
	//TODO: validate skin has been purchased
	return true;
}

void AGSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// Attribute change callbacks
		HealthChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSetBase->GetHealthAttribute()).AddUObject(this, &AGSPlayerState::HealthChanged);

		// Tag change callbacks
		//Leaving as example
		//AbilitySystemComponent->RegisterGameplayTagEvent(KnockedDownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AGSPlayerState::KnockDownTagChanged);
	}
}

void AGSPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	// Check for and handle knockdown and death
	AGravRacerPawn* Hero = Cast<AGravRacerPawn>(GetPawn());
	if (IsValid(Hero) && !IsAlive() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag))
	{
		if (Hero)
		{
			Hero->FinishDying();
		}
	}
}

int32 AGSPlayerState::GetTeamNum()
{
	//TODO:
	return 0;
}

void AGSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AGSPlayerState, Character, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGSPlayerState, CharacterSkin, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGSPlayerState, Vehicle, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(AGSPlayerState, VehicleSkin, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME(AGSPlayerState, RespawnLocation);
}