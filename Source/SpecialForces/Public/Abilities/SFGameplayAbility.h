// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "UObject/ObjectMacros.h"
#include "GameplayAbilitySpec.h"
#include "SpecialForces/SpecialForces.h"
#include "SFGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API USFGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	// Abilities with this set will automatically activate when the input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	ESFAbilityInputID AbilityInputID = ESFAbilityInputID::None;
};
