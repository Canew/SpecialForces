// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "SFAIController.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API ASFAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASFAIController();

	virtual void BeginPlay();

private:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

public:
	FName TargetActorKey;
		
private:
	UPROPERTY()
	class UAISenseConfig_Sight* SightSenseConfig;
	UPROPERTY()
	class UAISenseConfig_Damage* DamageSenseConfig;
	UPROPERTY()
	class UAISenseConfig_Hearing* HearingSenseConfig;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBlackboardData* BlackboardData;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;	
};
