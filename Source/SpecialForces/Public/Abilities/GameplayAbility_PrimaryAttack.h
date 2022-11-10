// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/SFGameplayAbility.h"
#include "GameplayAbility_PrimaryAttack.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API UGameplayAbility_PrimaryAttack : public USFGameplayAbility
{
	GENERATED_BODY()

public:
	UGameplayAbility_PrimaryAttack(const FObjectInitializer& ObjectInitializer);

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

protected:
	UFUNCTION()
	virtual void OnBlendOut();
	UFUNCTION()
	virtual void OnCancelled();
	UFUNCTION()
	virtual void OnCompleted();
	UFUNCTION()
	virtual void OnInterrupted();

private:
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* PrimaryAttackMontage;
};
