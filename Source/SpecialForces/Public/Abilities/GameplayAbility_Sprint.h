// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/SFGameplayAbility.h"
#include "GameplayAbility_Sprint.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API UGameplayAbility_Sprint : public USFGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_Sprint();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
};
