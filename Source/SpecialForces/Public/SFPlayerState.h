// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "SFPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API ASFPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASFPlayerState();

	// Implement IAbilitySystemInterface
	class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class USFAttributeSet* GetAttributeSet() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	class USFAbilitySystemComponent* AbilitySystem;

	UPROPERTY(VisibleAnywhere)
	class USFAttributeSet* AttributeSet;

	FDelegateHandle HealthChangedDelegateHandle;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
};
