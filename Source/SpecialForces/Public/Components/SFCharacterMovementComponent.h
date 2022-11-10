// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SFCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API USFCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	USFCharacterMovementComponent();

	virtual float GetMaxSpeed() const override;

	virtual bool IsSprinting() const;
	virtual bool IsAimDownSights() const;

protected:
	float SprintSpeedMultiplier;
	float AimDownSightsSpeedMultiplier;

	float SprintLeftThreshold;
	float SprintRightThreshold;
};
