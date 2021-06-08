// Copyright 2020 Dan Kestranek.


#include "Characters/Abilities/AttributeSets/GSAttributeSetMobility.h"
#include "Characters/GSCharacterBase.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Player/GSPlayerController.h"

UGSAttributeSetMobility::UGSAttributeSetMobility()
{

}


void UGSAttributeSetMobility::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UGSAttributeSetMobility::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	// If a Max value changes, adjust current to keep Current % of Current to Max
	if (Attribute == GetMaxFuelAttribute()) // GetMaxHealthAttribute comes from the Macros defined at the top of the header
	{
		AdjustAttributeForMaxChange(Fuel, MaxFuel, NewValue, GetFuelAttribute());
	}
	
}


void UGSAttributeSetMobility::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetFuelAttribute())
	{
		SetFuel(FMath::Clamp(GetFuel(), 0.0f, GetMaxFuel()));
	}
}

void UGSAttributeSetMobility::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGSAttributeSetMobility, Fuel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGSAttributeSetMobility, MaxFuel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGSAttributeSetMobility, FuelRegenRate, COND_None, REPNOTIFY_Always);
	
}

void UGSAttributeSetMobility::OnRep_Fuel(const FGameplayAttributeData& OldFuel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGSAttributeSetMobility, Fuel, OldFuel);
}

void UGSAttributeSetMobility::OnRep_MaxFuel(const FGameplayAttributeData& OldMaxFuel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGSAttributeSetMobility, MaxFuel, OldMaxFuel);
}

void UGSAttributeSetMobility::OnRep_FuelRegenRate(const FGameplayAttributeData& OldFuelRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGSAttributeSetMobility, FuelRegenRate, OldFuelRegenRate);
}