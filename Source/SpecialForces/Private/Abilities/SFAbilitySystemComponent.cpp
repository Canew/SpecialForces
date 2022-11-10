// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SFAbilitySystemComponent.h"

FGameplayAbilitySpec* USFAbilitySystemComponent::FindAbilitySpecFromClass(TSubclassOf<UGameplayAbility> InAbilityClass, bool bFindChildClass)
{
	if (bFindChildClass)
	{
		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			if (Spec.Ability->GetClass() == InAbilityClass || Spec.Ability->GetClass()->IsChildOf(InAbilityClass))
			{
				return &Spec;
			}
		}

		return nullptr;
	}
	else
	{
		return Super::FindAbilitySpecFromClass(InAbilityClass);
	}
}