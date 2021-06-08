// Copyright 2020 Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GSAttributeSetMobility.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class GRAVRACER_API UGSAttributeSetMobility : public UAttributeSet
{
	GENERATED_BODY()
	

public:
	UGSAttributeSetMobility();

	// Current Fuel, used to execute special abilities. Capped by MaxFuel.
	UPROPERTY(BlueprintReadOnly, Category = "Fuel", ReplicatedUsing = OnRep_Fuel)
	FGameplayAttributeData Fuel;
	ATTRIBUTE_ACCESSORS(UGSAttributeSetMobility, Fuel)

	// MaxFuel is its own attribute since GameplayEffects may modify it
	UPROPERTY(BlueprintReadOnly, Category = "Fuel", ReplicatedUsing = OnRep_MaxFuel)
	FGameplayAttributeData MaxFuel;
	ATTRIBUTE_ACCESSORS(UGSAttributeSetMobility, MaxFuel)

	// Fuel regen rate will passively increase Fuel every second
	UPROPERTY(BlueprintReadOnly, Category = "Fuel", ReplicatedUsing = OnRep_FuelRegenRate)
	FGameplayAttributeData FuelRegenRate;
	ATTRIBUTE_ACCESSORS(UGSAttributeSetMobility, FuelRegenRate)

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	UFUNCTION()
	virtual void OnRep_Fuel(const FGameplayAttributeData& OldFuel);

	UFUNCTION()
	virtual void OnRep_MaxFuel(const FGameplayAttributeData& OldMaxFuel);

	UFUNCTION()
	virtual void OnRep_FuelRegenRate(const FGameplayAttributeData& OldFuelRegenRate);
};
