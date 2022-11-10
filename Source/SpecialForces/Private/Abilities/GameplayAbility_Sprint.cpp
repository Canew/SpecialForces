// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_Sprint.h"

UGameplayAbility_Sprint::UGameplayAbility_Sprint()
{
	AbilityInputID = ESFAbilityInputID::Sprint;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Sprint"))));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Sprint"))));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.AimDownSights"))));
}

void UGameplayAbility_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}
	}
}

void UGameplayAbility_Sprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}