// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/GameplayAbility_Gunfire.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "SFCharacter.h"
#include "SFFirearm.h"
#include "Runtime/Engine/Classes/GameFramework/InputSettings.h"

UGameplayAbility_Gunfire::UGameplayAbility_Gunfire(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityInputID = ESFAbilityInputID::PrimaryAction;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.Ability.Gunfire"))));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Sprint"))));
}

bool UGameplayAbility_Gunfire::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

}

void UGameplayAbility_Gunfire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

		UAnimMontage* FireMontage = HipFireMontage;
		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		if (ASC)
		{
			if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.AimDownSights")))))
			{
				FireMontage = AimDownSightsFireMontage;

			}
		}
		if (!FireMontage)
		{
			return;
		}

		UAbilityTask_PlayMontageAndWait* AbilityTask_PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, FireMontage);
		AbilityTask_PlayMontageAndWait->OnBlendOut.AddDynamic(this, &UGameplayAbility_Gunfire::OnBlendOut);
		AbilityTask_PlayMontageAndWait->OnCancelled.AddDynamic(this, &UGameplayAbility_Gunfire::OnCancelled);
		AbilityTask_PlayMontageAndWait->OnCompleted.AddDynamic(this, &UGameplayAbility_Gunfire::OnCompleted);
		AbilityTask_PlayMontageAndWait->OnInterrupted.AddDynamic(this, &UGameplayAbility_Gunfire::OnInterrupted);
		AbilityTask_PlayMontageAndWait->ReadyForActivation();

		ASFWeapon* CharacterWeapon = Character->GetEquipWeapon();
		if (!CharacterWeapon)
		{
			return;
		}

		CharacterWeapon->DoAttack();

		if (CharacterWeapon->IsEnableAutoAttack())
		{
			UWorld* World = GetWorld();
			if (!World)
			{
				return;
			}

			World->GetTimerManager().SetTimer(AutoAttackTimerHandle, FTimerDelegate::CreateLambda(
				[this]() -> void {
					if (IsPressInput())
					{
						UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
						if (ASC)
						{
							ASC->TryActivateAbilityByClass(this->GetClass(), true);
						}
					}
				}
			), CharacterWeapon->GetAutoAttackInterval(), false, CharacterWeapon->GetAutoAttackInterval());
		}
	}
}

void UGameplayAbility_Gunfire::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		ASFCharacter* SFCharacter = Cast<ASFCharacter>(ActorInfo->AvatarActor);
		if (!SFCharacter)
		{
			return;
		}

		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

void UGameplayAbility_Gunfire::OnBlendOut()
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), true, false);
}

void UGameplayAbility_Gunfire::OnCancelled()
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), true, true);
}

void UGameplayAbility_Gunfire::OnCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), true, false);
}

void UGameplayAbility_Gunfire::OnInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), true, false);
}

void UGameplayAbility_Gunfire::OnRelease(float TimeHeld)
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(AutoAttackTimerHandle);
	}

	bInputReleased = true;
}

bool UGameplayAbility_Gunfire::IsPressInput()
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return false;
	}

	if (Character->IsPlayerControlled())
	{
		const UInputSettings* InputSettings = GetDefault<UInputSettings>();
		TArray<FInputActionKeyMapping> InputActionKeyMappings;
		InputSettings->GetActionMappingByName(FName(TEXT("PrimaryAction")), InputActionKeyMappings);

		APlayerController* PC = Cast<APlayerController>(Character->GetController());
		for (FInputActionKeyMapping InputActionKeyMapping : InputActionKeyMappings)
		{
			if (PC->IsInputKeyDown(InputActionKeyMapping.Key))
			{
				return true;
			}
		}
	}

	return false;
}
