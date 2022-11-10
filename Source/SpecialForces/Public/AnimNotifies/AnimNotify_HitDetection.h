// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_HitDetection.generated.h"

UENUM()
enum class EHitDetectionMethod : uint8
{
	Trace,
	Dot
};

UENUM()
enum class ETraceShape : uint8
{
	Line,
	Box,
	Sphere,
	Capsule
};

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API UAnimNotify_HitDetection : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_HitDetection();

	FString GetNotifyName_Implementation() const;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
	
protected:
	UPROPERTY(EditAnywhere)
	EHitDetectionMethod HitDetectionMethod;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "HitDetectionMethod == EHitDetectionMethod::Trace", EditConditionHides))
	ETraceShape TraceShape;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "HitDetectionMethod == EHitDetectionMethod::Trace", EditConditionHides))
	float TraceRange;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "HitDetectionMethod == EHitDetectionMethod::Trace && TraceShape == ETraceShape::Box", EditConditionHides))
	FVector BoxHalfExtent;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "HitDetectionMethod == EHitDetectionMethod::Trace && TraceShape == ETraceShape::Sphere", EditConditionHides))
	float SphereRadius;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "HitDetectionMethod == EHitDetectionMethod::Trace && TraceShape == ETraceShape::Capsule", EditConditionHides))
	float CapsuleRadius;
	UPROPERTY(EditAnywhere, Meta = (EditCondition = "HitDetectionMethod == EHitDetectionMethod::Trace && TraceShape == ETraceShape::Capsule", EditConditionHides))
	float CapsuleHalfHeight;

	UPROPERTY(EditAnywhere, Meta = (EditCondition = "HitDetectionMethod == EHitDetectionMethod::Dot", EditConditionHides, ClampMin = "0.0"))
	float HitRange;
	UPROPERTY(EditAnywhere, Meta = (EditCondition = "HitDetectionMethod == EHitDetectionMethod::Dot", EditConditionHides, ClampMin = "0.0", ClampMax = "180.0"))
	float HitAngle;

	UPROPERTY(EditAnywhere)
	FCollisionProfileName CollisionProfile;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGameplayEffect> HitEffectClass;
};
