// Fill out your copyright notice in the Description page of Project Settings.


#include "SFAnimInstance.h"
#include "SFCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/SFCharacterMovementComponent.h"
#include "Abilities/SFAbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "Kismet/KismetMathLibrary.h"

void USFAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void USFAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ASFCharacter* Character = Cast<ASFCharacter>(TryGetPawnOwner());
	if (Character)
	{
		Character->ShowHitReact.AddDynamic(this, &USFAnimInstance::StartHitReact);
	}
}

void USFAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* Pawn = TryGetPawnOwner();
	if (Pawn)
	{
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();
		Speed = Pawn->GetVelocity().Size();

		FRotator RotatorDeltaActorAndControl = UKismetMathLibrary::NormalizedDeltaRotator(Pawn->GetBaseAimRotation(), Pawn->GetActorRotation());
		Roll = RotatorDeltaActorAndControl.Roll;
		Pitch = RotatorDeltaActorAndControl.Pitch;
		Yaw = RotatorDeltaActorAndControl.Yaw;

		FRotator RotatorDeltaActorAndLastTick = UKismetMathLibrary::NormalizedDeltaRotator(RotationLastTick, Pawn->GetActorRotation());
		YawDelta = FMath::FInterpTo(YawDelta, RotatorDeltaActorAndLastTick.Yaw / DeltaSeconds / LeanIntensityScale, DeltaSeconds, LeanInterpolationSpeed);
		RotationLastTick = Pawn->GetActorRotation();

		FVector ForwardVector = FVector(Pawn->GetActorForwardVector().X, Pawn->GetActorForwardVector().Y, 0.0f);
		FVector MoveVelocity = FVector(Pawn->GetVelocity().X, Pawn->GetVelocity().Y, 0.0f);
		ForwardVector.Normalize();
		MoveVelocity.Normalize();
		MoveDirection = FMath::RadiansToDegrees(FMath::Acos(ForwardVector | MoveVelocity));

		float DistanceToFrontBackPlane = FVector::PointPlaneDist(Pawn->GetActorLocation() + Pawn->GetVelocity(), Pawn->GetActorLocation(), Pawn->GetActorRightVector());
		if (DistanceToFrontBackPlane < 0)
		{
			MoveDirection *= -1.0f;
		}

		ASFCharacter* Character = Cast<ASFCharacter>(Pawn);
		if (Character)
		{
			bIsAccelerating = Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f;
			bFullBody = GetCurveValue(FName(TEXT("FullBody"))) > 0.0f;
			bIsCrouching = Character->bIsCrouched;

			USFCharacterMovementComponent* CMC = Cast<USFCharacterMovementComponent>(Character->GetCharacterMovement());
			if (CMC)
			{
				bIsSprinting = CMC->IsSprinting();
			}

			UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
			if (ASC)
			{
				bIsAimDownSights = ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.AimDownSights"))));
			}

			if (HitReactDirectionFlag == ESFDirection::None)
			{
				HitReactDirection = ESFDirection::None;
			}
			else
			{
				HitReactDirection = HitReactDirectionFlag;
				HitReactDirectionFlag = ESFDirection::None;
			}
		}
	}
}

void USFAnimInstance::StartHitReact(ESFDirection NewHitReactDirection)
{
	HitReactDirectionFlag = NewHitReactDirection;
}
