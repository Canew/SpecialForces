// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/SFGameplayAbility.h"
#include "Components/TimeLineComponent.h"
#include "GameplayAbility_AimDownSights.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API UGameplayAbility_AimDownSights : public USFGameplayAbility
{
	GENERATED_BODY()

public:
	UGameplayAbility_AimDownSights();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	virtual void InitializeAimDownSightsTimeline();

	virtual void StartAimDownSights();
	virtual void StopAimDownSights();

	UFUNCTION()
	virtual void TickAimDownSightsLength(float Length);
	UFUNCTION()
	virtual void TickAimDownSightsLocation(FVector Location);

	UFUNCTION()
	virtual void FinishedTimeline();

protected:
	FTimeline AimDownSightsTimeline;

	UPROPERTY(EditDefaultsOnly)
	class UCurveFloat* AimDownSightsLengthCurve;

	UPROPERTY(EditDefaultsOnly)
	class UCurveVector* AimDownSightsLocationCurve;

	float AimDownSightsTimelineLength;
	float OriginTargetArmLength;
	float AimDownSightsTargetArmLength;
	FVector OriginTargetArmLocation;
	FVector AimDownSightsTargetArmLocation;

	bool bInitializedAimDownSightsTimeline;

	FTimerHandle TimelineTickTimerHandle;

	class ASFCharacter* OwningCharacter;
};
