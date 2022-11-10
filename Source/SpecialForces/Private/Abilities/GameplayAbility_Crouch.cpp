// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_Crouch.h"
#include "Components/SFCharacterMovementComponent.h"
#include "SFCharacter.h"

UGameplayAbility_Crouch::UGameplayAbility_Crouch()
{
	AbilityInputID = ESFAbilityInputID::Crouch;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	//AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Crouch"))));
	//ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Crouch"))));
}

void UGameplayAbility_Crouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}

		ASFCharacter* Character = Cast<ASFCharacter>(ActorInfo->AvatarActor);
		if (!Character)
		{
			return;
		}

		USFCharacterMovementComponent* CMC = Cast<USFCharacterMovementComponent>(Character->GetCharacterMovement());
		if (!CMC)
		{
			return;
		}

		if (CMC->IsCrouching())
		{
			Character->UnCrouch();
		}
		else
		{
			Character->Crouch();
		}

		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}