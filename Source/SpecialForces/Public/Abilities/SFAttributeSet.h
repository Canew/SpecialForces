// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpecialForces/SpecialForces.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SFAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAttributeChangedSignature, float, Attribute, int32, StackCount);

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API USFAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	USFAttributeSet();

	// Define Attributes
	/** Attribute for Health */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(USFAttributeSet, Health)

	/** Attribute for Max Health */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USFAttributeSet, MaxHealth)

	/** Attribute for BaseSpeed */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_BaseSpeed)
	FGameplayAttributeData BaseSpeed;
	ATTRIBUTE_ACCESSORS(USFAttributeSet, BaseSpeed)

	/** Attribute for SpeedMultiplier */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_SpeedMultiplier)
	FGameplayAttributeData SpeedMultiplier;
	ATTRIBUTE_ACCESSORS(USFAttributeSet, SpeedMultiplier)

	/** Attribute for Damage */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(USFAttributeSet, Damage)

	/** Delegates for attribute changes */
	FAttributeChangedSignature HealthChangedDelegate;
	FAttributeChangedSignature MaxHealthChangedDelegate;
	FAttributeChangedSignature BaseSpeedChangedDelegate;
	FAttributeChangedSignature SpeedMultiplierChangedDelegate;

	/** Post Gameplay Effect Execute, only to clamp values */
	void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Replication Functions for Health and Mana */
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION()
	virtual void OnRep_BaseSpeed(const FGameplayAttributeData& OldBaseSpeed);
	UFUNCTION()
	virtual void OnRep_SpeedMultiplier(const FGameplayAttributeData& OldSpeedMultiplier);

private:
	FGameplayTag HitDirectionFrontTag;
	FGameplayTag HitDirectionBackTag;
	FGameplayTag HitDirectionRightTag;
	FGameplayTag HitDirectionLeftTag;
};
