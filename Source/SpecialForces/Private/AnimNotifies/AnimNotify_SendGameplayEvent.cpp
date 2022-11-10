// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifies/AnimNotify_SendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "SFCharacter.h"

FString UAnimNotify_SendGameplayEvent::GetNotifyName_Implementation() const
{
	return FString(TEXT("SendGameplayEvent"));
}

void UAnimNotify_SendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ASFCharacter* SFCharacter = Cast<ASFCharacter>(MeshComp->GetOwner());
	if (SFCharacter)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(SFCharacter, TagToSend, FGameplayEventData());
	}
}
