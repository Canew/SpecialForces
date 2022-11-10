// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_HitDetection.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

UAnimNotify_HitDetection::UAnimNotify_HitDetection()
{
	bIsNativeBranchingPoint = true;
}

FString UAnimNotify_HitDetection::GetNotifyName_Implementation() const
{
	if (HitDetectionMethod == EHitDetectionMethod::Trace)
	{
		return TEXT("Hit Detection with Trace");
	}
	else if (HitDetectionMethod == EHitDetectionMethod::Dot)
	{
		return TEXT("Hit Detection with Dot");
	}
	else
	{
		return TEXT("Hit Detection");
	}
}

void UAnimNotify_HitDetection::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	UAbilitySystemComponent* Source = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Owner);
	if (!Source)
	{
		return;
	}

	UWorld* World = Owner->GetWorld();
	if (!World)
	{
		return;
	}


	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(Owner);
	if (HitDetectionMethod == EHitDetectionMethod::Trace)
	{
		TArray<FHitResult> HitResults;
		switch (TraceShape)
		{
		case ETraceShape::Line:
		{
			World->LineTraceMultiByProfile(HitResults, Owner->GetActorLocation(), Owner->GetActorLocation() + TraceRange * Owner->GetActorForwardVector(), CollisionProfile.Name, CollisionQueryParams);
			break;
		}

		case ETraceShape::Box:
		{
			World->SweepMultiByProfile(HitResults, Owner->GetActorLocation(), Owner->GetActorLocation() + TraceRange * Owner->GetActorForwardVector(), FQuat::Identity, CollisionProfile.Name, FCollisionShape::MakeBox(BoxHalfExtent), CollisionQueryParams);
			break;
		}

		case ETraceShape::Sphere:
		{
			World->SweepMultiByProfile(HitResults, Owner->GetActorLocation(), Owner->GetActorLocation() + TraceRange * Owner->GetActorForwardVector(), FQuat::Identity, CollisionProfile.Name, FCollisionShape::MakeSphere(SphereRadius), CollisionQueryParams);
			break;
		}

		case ETraceShape::Capsule:
		{
			World->SweepMultiByProfile(HitResults, Owner->GetActorLocation(), Owner->GetActorLocation() + TraceRange * Owner->GetActorForwardVector(), FQuat::Identity, CollisionProfile.Name, FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight), CollisionQueryParams);
			break;
		}
		}

		for (FHitResult& HitResult : HitResults)
		{
			if (HitResult.GetActor())
			{
				UAbilitySystemComponent* Target = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitResult.GetActor());
				if (Target && HitEffectClass)
				{
					Source->ApplyGameplayEffectToTarget(HitEffectClass.GetDefaultObject(), Target);
				}
			}
		}
	}
	else if (HitDetectionMethod == EHitDetectionMethod::Dot)
	{
		TArray<FOverlapResult> OverlapResults;
		World->OverlapMultiByProfile(OverlapResults, Owner->GetActorLocation(), FQuat::Identity, CollisionProfile.Name, FCollisionShape::MakeSphere(HitRange), CollisionQueryParams);

		for (FOverlapResult& OverlapResult : OverlapResults)
		{
			if (OverlapResult.GetActor())
			{
				UAbilitySystemComponent* Target = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OverlapResult.GetActor());
				if (Target && HitEffectClass)
				{
					Source->ApplyGameplayEffectToTarget(HitEffectClass.GetDefaultObject(), Target);
				}
			}
		}
	}
}