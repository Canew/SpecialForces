// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpecialForces/SpecialForces.h"
#include "Animation/AnimInstance.h"
#include "SFAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API USFAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UFUNCTION()
	virtual void StartHitReact(ESFDirection NewHitReactDirection);

private:
	/** True if character is in air. */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsInAir;

	/** Velocity size of character */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float Speed;

	/** Delta that character rotation and control rotation */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float Roll;
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float Pitch;
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float Yaw;

	/** Delta that current yaw and yaw at last frame */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float YawDelta;

	/** Character's move direction */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	float MoveDirection;

	/** Value for proper tilt of character */
	float LeanIntensityScale = 7.0f;
	float LeanInterpolationSpeed = 6.0f;

	/** Store FRotator at last tick */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	FRotator RotationLastTick;

	/** True if character's velocity is being increased */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsSprinting;

	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsCrouching;

	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsAimDownSights;

	/** True if character is dead */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bIsDead;

	/** if character should play full body animation */
	UPROPERTY(BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool bFullBody;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	ESFDirection HitReactDirection;
	ESFDirection HitReactDirectionFlag;
};
