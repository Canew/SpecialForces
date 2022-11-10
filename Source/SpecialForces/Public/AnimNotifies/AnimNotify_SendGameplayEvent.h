// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_SendGameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API UAnimNotify_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
	
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	UPROPERTY(EditAnywhere, Category = "Tags")
	FGameplayTag TagToSend;
};
