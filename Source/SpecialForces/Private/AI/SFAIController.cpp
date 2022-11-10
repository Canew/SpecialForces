// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SFAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"

ASFAIController::ASFAIController()
{
	bWantsPlayerState = true;

	// Setup the perception component
	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception")));

	SightSenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightSenseConfig"));
	SightSenseConfig->SightRadius = 1500.0f;
	SightSenseConfig->LoseSightRadius = 2000.0f;
	SightSenseConfig->SetMaxAge(15.0f);
	GetPerceptionComponent()->ConfigureSense(*SightSenseConfig);

	DamageSenseConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageSenseConfig"));
	DamageSenseConfig->SetMaxAge(30.0f);
	GetPerceptionComponent()->ConfigureSense(*DamageSenseConfig);

	HearingSenseConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingSenseConfig"));
	HearingSenseConfig->HearingRange = 1500.0f;
	HearingSenseConfig->SetMaxAge(10.0f);
	GetPerceptionComponent()->ConfigureSense(*HearingSenseConfig);

	GetPerceptionComponent()->SetDominantSense(SightSenseConfig->GetSenseImplementation());

	SetGenericTeamId(FGenericTeamId(1));

	TargetActorKey = TEXT("TargetActor");
}

void ASFAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BlackboardData && BehaviorTree)
	{
		if (UseBlackboard(BlackboardData, Blackboard))
		{
			RunBehaviorTree(BehaviorTree);
		}
	}

	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ASFAIController::OnTargetPerceptionUpdated);
}

void ASFAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (Blackboard)
		{
			TArray<AActor*> HostileActors;
			GetAIPerceptionComponent()->GetHostileActors(HostileActors);
			if (HostileActors.Num() > 0)
			{
				Blackboard->SetValueAsObject(TargetActorKey, HostileActors[FMath::RandRange(0, HostileActors.Num() - 1)]);
			}
		}
	}
	else
	{
		if (Blackboard)
		{
			TArray<AActor*> HostileActors;
			GetAIPerceptionComponent()->GetHostileActors(HostileActors);
			if (HostileActors.Num() == 0)
			{
				Blackboard->SetValueAsObject(TargetActorKey, nullptr);
			}
		}
	}
}