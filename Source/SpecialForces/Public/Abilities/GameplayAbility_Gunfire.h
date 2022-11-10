// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/SFGameplayAbility.h"
#include "GameplayAbility_Gunfire.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API UGameplayAbility_Gunfire : public USFGameplayAbility
{
	GENERATED_BODY()

public:
	UGameplayAbility_Gunfire(const FObjectInitializer& ObjectInitializer);

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UFUNCTION()
	virtual void OnBlendOut();
	UFUNCTION()
	virtual void OnCancelled();
	UFUNCTION()
	virtual void OnCompleted();
	UFUNCTION()
	virtual void OnInterrupted();
	UFUNCTION()
	virtual void OnRelease(float TimeHeld);

	virtual bool IsPressInput();

private:
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* HipFireMontage;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* AimDownSightsFireMontage;

	FTimerHandle AutoAttackTimerHandle;

	bool bInputReleased = false;
};
