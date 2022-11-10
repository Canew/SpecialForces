// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SFAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API USFAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	FGameplayAbilitySpec* FindAbilitySpecFromClass(TSubclassOf<UGameplayAbility> InAbilityClass, bool bFindChildClass);
};
