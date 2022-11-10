// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTree/BTDecorator_IsInAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/SFAIController.h"
#include "SFCharacter.h"

UBTDecorator_IsInAttackRange::UBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("Is In Attack Range");
}

bool UBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	ASFCharacter* Character = Cast<ASFCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	ASFAIController* AIController = Cast<ASFAIController>(OwnerComp.GetAIOwner());
	if (!Character || !AIController)
	{
		return false;
	}

	AActor* Target = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AIController->TargetActorKey));
	if (Target == nullptr)
	{
		return false;
	}

	bResult = (Character->GetDistanceTo(Target) <= Character->GetPrimaryAttackRange());

	return bResult;
}

FString UBTDecorator_IsInAttackRange::GetStaticDescription() const
{
	if (IsInversed())
	{
		return TEXT("Is not In Attack Range");
	}
	else
	{
		return TEXT("Is In Attack Range");
	}
}
