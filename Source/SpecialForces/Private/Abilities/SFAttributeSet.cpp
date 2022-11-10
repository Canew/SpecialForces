// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/SFAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "SFCharacter.h"
#include "SFGameplayPlayerController.h"

USFAttributeSet::USFAttributeSet()
{
	HitDirectionFrontTag = FGameplayTag::RequestGameplayTag(TEXT("Character.HitReact.Front"));
	HitDirectionBackTag = FGameplayTag::RequestGameplayTag(TEXT("Character.HitReact.Front"));
	HitDirectionRightTag = FGameplayTag::RequestGameplayTag(TEXT("Character.HitReact.Front"));
	HitDirectionLeftTag = FGameplayTag::RequestGameplayTag(TEXT("Character.HitReact.Front"));
}

void USFAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	ASFCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<ASFCharacter>(TargetActor);
	}

	// Get the Source actor
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	ASFCharacter* SourceCharacter = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (SourceController)
		{
			SourceCharacter = Cast<ASFCharacter>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<ASFCharacter>(SourceActor);
		}

		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
		HealthChangedDelegate.Broadcast(GetHealth(), Data.EffectSpec.StackCount);
	}

	if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetMaxHealth(FMath::Clamp(GetMaxHealth(), 0.0f, GetMaxHealth()));
		MaxHealthChangedDelegate.Broadcast(GetMaxHealth(), Data.EffectSpec.StackCount);
	}

	if (Data.EvaluatedData.Attribute == GetBaseSpeedAttribute())
	{
		SetBaseSpeed(FMath::Clamp(GetBaseSpeed(), 0.0f, GetBaseSpeed()));
		BaseSpeedChangedDelegate.Broadcast(GetBaseSpeed(), Data.EffectSpec.StackCount);
	}

	if (Data.EvaluatedData.Attribute == GetSpeedMultiplierAttribute())
	{
		SetSpeedMultiplier(FMath::Clamp(GetSpeedMultiplier(), 0.1f, GetSpeedMultiplier()));
		SpeedMultiplierChangedDelegate.Broadcast(GetSpeedMultiplier(), Data.EffectSpec.StackCount);
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Store a local copy of the amount of damage done and clear the damage attribute
		float LocalDamageDone = GetDamage();
		SetDamage(0.0f);

		if (TargetCharacter && !TargetCharacter->IsDead())
		{
			const FHitResult* HitResult = Context.GetHitResult();

			// Show damage number for the Source or Target player unless it was self damage
			if (SourceActor != TargetActor)
			{
				ASFGameplayPlayerController* SourcePC = Cast<ASFGameplayPlayerController>(SourceController);
				if (SourcePC)
				{
					if (HitResult)
					{
						FClosestPointOnPhysicsAsset ClosestPointOnPhysicsAsset;
						TargetCharacter->GetMesh()->GetClosestPointOnPhysicsAsset(HitResult->Location, ClosestPointOnPhysicsAsset, false);
						LocalDamageDone *= TargetCharacter->GetDamageMultiplierByBodyParts(ClosestPointOnPhysicsAsset.BoneName);
						SourcePC->ShowDamageText(LocalDamageDone, TargetCharacter, HitResult->Location);
					}
					else
					{
						SourcePC->ShowDamageText(LocalDamageDone, TargetCharacter);
					}
				}

				ASFGameplayPlayerController* TargetPC = Cast<ASFGameplayPlayerController>(TargetController);
				if (TargetPC)
				{
					if (HitResult)
					{
						FClosestPointOnPhysicsAsset ClosestPointOnPhysicsAsset;
						TargetCharacter->GetMesh()->GetClosestPointOnPhysicsAsset(HitResult->Location, ClosestPointOnPhysicsAsset, false);
						LocalDamageDone *= TargetCharacter->GetDamageMultiplierByBodyParts(ClosestPointOnPhysicsAsset.BoneName);
						TargetPC->ShowDamageText(LocalDamageDone, TargetCharacter, HitResult->Location, FColor::Red);
					}
					else
					{
						TargetPC->ShowDamageText(LocalDamageDone, TargetCharacter, FVector::ZeroVector, FColor::Red);
					}
				}
			}

			// Play HitReact animation and sound with a multicast RPC.
			if (HitResult)
			{
				ESFDirection HitDirection = TargetCharacter->GetHitReactDirection(Context.GetHitResult()->Location);
				if (HitDirection != ESFDirection::None)
				{
					TargetCharacter->PlayHitReact(HitDirection, SourceCharacter);
				}
			}
			else
			{
				// No hit result. Default to front.
				TargetCharacter->PlayHitReact(ESFDirection::Front, SourceCharacter);
			}
		}

		// Apply the health change and then clamp it
		const float NewHealth = GetHealth() - LocalDamageDone;
		SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

		UAISense_Damage::ReportDamageEvent(GetWorld(), TargetActor, SourceActor, LocalDamageDone, SourceActor->GetActorLocation(), TargetActor->GetActorLocation());
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), TargetActor->GetActorLocation(), 0.2f, SourceActor, 0.0f);
	}
}

void USFAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(USFAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USFAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USFAttributeSet, BaseSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USFAttributeSet, SpeedMultiplier, COND_None, REPNOTIFY_Always);
}

void USFAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USFAttributeSet, Health, OldHealth);
}

void USFAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USFAttributeSet, MaxHealth, OldMaxHealth);
}

void USFAttributeSet::OnRep_BaseSpeed(const FGameplayAttributeData& OldBaseSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USFAttributeSet, BaseSpeed, OldBaseSpeed);
}

void USFAttributeSet::OnRep_SpeedMultiplier(const FGameplayAttributeData& OldSpeedMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USFAttributeSet, SpeedMultiplier, OldSpeedMultiplier);
}