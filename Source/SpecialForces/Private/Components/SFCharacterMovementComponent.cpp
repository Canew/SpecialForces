// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SFCharacterMovementComponent.h"
#include "SFCharacter.h"
#include "Abilities/SFAbilitySystemComponent.h"
#include "GameplayTagContainer.h"

USFCharacterMovementComponent::USFCharacterMovementComponent()
{
	SprintSpeedMultiplier = 1.6f;
	AimDownSightsSpeedMultiplier = 0.4f;

	SprintLeftThreshold = -60.0f;
	SprintRightThreshold = 60.0f;
}

float USFCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();

	ASFCharacter* Owner = Cast<ASFCharacter>(GetOwner());
	if (!Owner)
	{
		return MaxSpeed;
	}

	if (IsSprinting())
	{
		return MaxSpeed *= SprintSpeedMultiplier;
	}

	if (IsAimDownSights())
	{
		return MaxSpeed *= AimDownSightsSpeedMultiplier;
	}

	return MaxSpeed;
}

bool USFCharacterMovementComponent::IsSprinting() const
{
	bool IsSprinting = true;

	ASFCharacter* Owner = Cast<ASFCharacter>(GetOwner());
	if (Owner)
	{
		if (Velocity.Size() > 0.0f)
		{
			IsSprinting &= true;
		}
		else
		{
			return false;
		}

		FVector ForwardVector = FVector(Owner->GetActorForwardVector().X, Owner->GetActorForwardVector().Y, 0.0f);
		FVector MoveVelocity = FVector(Velocity.X, Velocity.Y, 0.0f);
		ForwardVector.Normalize();
		MoveVelocity.Normalize();
		float MoveDirection = FMath::RadiansToDegrees(FMath::Acos(ForwardVector | MoveVelocity));

		bool bRight = FVector::CrossProduct(ForwardVector, MoveVelocity).Equals(FVector::UpVector);
		MoveDirection = bRight ? MoveDirection : -1.0f * MoveDirection;
		if (MoveDirection >= SprintLeftThreshold && MoveDirection <= SprintRightThreshold)
		{
			IsSprinting &= true;
		}
		else
		{
			return false;
		}

		UAbilitySystemComponent* ASC = Owner->GetAbilitySystemComponent();
		if (ASC)
		{
			IsSprinting &= ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Sprint"))));
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return IsSprinting;
}

bool USFCharacterMovementComponent::IsAimDownSights() const
{
	ASFCharacter* Owner = Cast<ASFCharacter>(GetOwner());
	if (Owner)
	{
		UAbilitySystemComponent* ASC = Owner->GetAbilitySystemComponent();
		if (ASC)
		{
			return ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.AimDownSights"))));
		}
	}

	return false;
}
