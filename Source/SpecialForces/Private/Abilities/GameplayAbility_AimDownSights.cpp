// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_AimDownSights.h"
#include "GameFramework/SpringArmComponent.h"
#include "SFCharacter.h"


UGameplayAbility_AimDownSights::UGameplayAbility_AimDownSights()
{
	AbilityInputID = ESFAbilityInputID::SecondaryAction;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.AimDownSights"))));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.AimDownSights"))));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Sprint"))));

	AimDownSightsTimelineLength = 0.5f;
	OriginTargetArmLength = 250.0f;
	AimDownSightsTargetArmLength = 65.0f;
	OriginTargetArmLocation = FVector(0.0f, 30.0f, 100.0f);
	AimDownSightsTargetArmLocation = FVector(0.0f, 45.0f, 90.0f);

	bInitializedAimDownSightsTimeline = false;
}

void UGameplayAbility_AimDownSights::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			return;
		}
	}

	ASFCharacter* Character = Cast<ASFCharacter>(ActorInfo->AvatarActor);
	if (Character)
	{
		OwningCharacter = Character;
		InitializeAimDownSightsTimeline();
		StartAimDownSights();
	}
	else
	{
		InputReleased(Handle, ActorInfo, ActivationInfo);
	}
}

void UGameplayAbility_AimDownSights::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UGameplayAbility_AimDownSights::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	ASFCharacter* Character = Cast<ASFCharacter>(ActorInfo->AvatarActor);
	if (Character)
	{
		StopAimDownSights();
	}
}

void UGameplayAbility_AimDownSights::InitializeAimDownSightsTimeline()
{
	if (AimDownSightsLengthCurve && AimDownSightsLocationCurve)
	{
		FOnTimelineFloat AimDownSightsLengthCurveCallback;
		AimDownSightsLengthCurveCallback.BindUFunction(this, TEXT("TickAimDownSightsLength"));
		AimDownSightsTimeline.AddInterpFloat(AimDownSightsLengthCurve, AimDownSightsLengthCurveCallback);

		FOnTimelineVector AimDownSightsLocationCurveCallback;
		AimDownSightsLocationCurveCallback.BindUFunction(this, TEXT("TickAimDownSightsLocation"));
		AimDownSightsTimeline.AddInterpVector(AimDownSightsLocationCurve, AimDownSightsLocationCurveCallback);

		FOnTimelineEvent TimelineFinishedEvent;
		TimelineFinishedEvent.BindUFunction(this, TEXT("FinishedTimeline"));
		AimDownSightsTimeline.SetTimelineFinishedFunc(TimelineFinishedEvent);

		bInitializedAimDownSightsTimeline = true;
	}
}

void UGameplayAbility_AimDownSights::StartAimDownSights()
{
	if (bInitializedAimDownSightsTimeline)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().SetTimer(TimelineTickTimerHandle, FTimerDelegate::CreateLambda(
				[this]() -> void
				{
					AimDownSightsTimeline.TickTimeline(GetWorld()->GetDeltaSeconds());
				}), World->GetDeltaSeconds(), true);
		}

		AimDownSightsTimeline.Play();
	}
	else
	{
		USpringArmComponent* CharacterSpringArm = OwningCharacter->GetCharacterSpringArm();
		if (CharacterSpringArm)
		{
			CharacterSpringArm->TargetArmLength = AimDownSightsTargetArmLength;
			CharacterSpringArm->SetRelativeLocation(AimDownSightsTargetArmLocation);
		}
	}
}

void UGameplayAbility_AimDownSights::StopAimDownSights()
{
	if (bInitializedAimDownSightsTimeline)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().SetTimer(TimelineTickTimerHandle, FTimerDelegate::CreateLambda(
				[this]() -> void
				{
					AimDownSightsTimeline.TickTimeline(GetWorld()->GetDeltaSeconds());
				}), World->GetDeltaSeconds(), true);
		}

		AimDownSightsTimeline.Reverse();
	}
	else
	{
		USpringArmComponent* CharacterSpringArm = OwningCharacter->GetCharacterSpringArm();
		if (CharacterSpringArm)
		{
			CharacterSpringArm->TargetArmLength = OriginTargetArmLength;
			CharacterSpringArm->SetRelativeLocation(OriginTargetArmLocation);
		}
	}
}

void UGameplayAbility_AimDownSights::TickAimDownSightsLength(float Length)
{
	USpringArmComponent* CharacterSpringArm = OwningCharacter->GetCharacterSpringArm();
	if (CharacterSpringArm)
	{
		CharacterSpringArm->TargetArmLength = Length;
	}
	
}

void UGameplayAbility_AimDownSights::TickAimDownSightsLocation(FVector Location)
{
	USpringArmComponent* CharacterSpringArm = OwningCharacter->GetCharacterSpringArm();
	if (CharacterSpringArm)
	{
		CharacterSpringArm->SetRelativeLocation(Location);
	}
}

void UGameplayAbility_AimDownSights::FinishedTimeline()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(TimelineTickTimerHandle);
	}
}