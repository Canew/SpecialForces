// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_PrimaryAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "SFCharacter.h"
#include "SFWeapon.h"

UGameplayAbility_PrimaryAttack::UGameplayAbility_PrimaryAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityInputID = ESFAbilityInputID::PrimaryAction;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.Ability.PrimaryAttack"))));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Sprint"))));
}

bool UGameplayAbility_PrimaryAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

}

void UGameplayAbility_PrimaryAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
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

		if (!PrimaryAttackMontage)
		{
			return;
		}

		UAbilityTask_PlayMontageAndWait* AbilityTask_PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PrimaryAttackMontage);
		AbilityTask_PlayMontageAndWait->OnBlendOut.AddDynamic(this, &UGameplayAbility_PrimaryAttack::OnBlendOut);
		AbilityTask_PlayMontageAndWait->OnCancelled.AddDynamic(this, &UGameplayAbility_PrimaryAttack::OnCancelled);
		AbilityTask_PlayMontageAndWait->OnCompleted.AddDynamic(this, &UGameplayAbility_PrimaryAttack::OnCompleted);
		AbilityTask_PlayMontageAndWait->OnInterrupted.AddDynamic(this, &UGameplayAbility_PrimaryAttack::OnInterrupted);
		AbilityTask_PlayMontageAndWait->ReadyForActivation();
	}
}

void UGameplayAbility_PrimaryAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);


}

void UGameplayAbility_PrimaryAttack::OnBlendOut()
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), true, false);
}

void UGameplayAbility_PrimaryAttack::OnCancelled()
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), true, true);
}

void UGameplayAbility_PrimaryAttack::OnCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), true, false);
}

void UGameplayAbility_PrimaryAttack::OnInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), true, false);
}
