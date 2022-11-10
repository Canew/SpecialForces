// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/SFGameplayAbility.h"
#include "GameplayAbility_Crouch.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API UGameplayAbility_Crouch : public USFGameplayAbility
{
	GENERATED_BODY()

	UGameplayAbility_Crouch();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
};
