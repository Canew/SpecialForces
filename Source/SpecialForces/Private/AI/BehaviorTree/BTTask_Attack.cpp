// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTree/BTTask_Attack.h"
#include "AI/SFAIController.h"
#include "Abilities/GameplayAbility_PrimaryAttack.h"
#include "Abilities/SFAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "SFCharacter.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = Super::ExecuteTask(OwnerComp, NodeMemory);

	ASFAIController* AIController = Cast<ASFAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	USFAbilitySystemComponent* ASC = Cast<USFAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn));
	if (!ASC)
	{
		return EBTNodeResult::Failed;
	}

	FGameplayAbilitySpec* PrimaryAttackAbilitySpec = ASC->FindAbilitySpecFromClass(UGameplayAbility_PrimaryAttack::StaticClass(), true);
	if (PrimaryAttackAbilitySpec)
	{
		ASC->TryActivateAbility(PrimaryAttackAbilitySpec->Handle);
	}
	else
	{
		return EBTNodeResult::Failed;
	}

	return NodeResult;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);


}