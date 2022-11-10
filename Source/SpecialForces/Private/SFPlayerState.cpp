// Fill out your copyright notice in the Description page of Project Settings.


#include "SFPlayerState.h"
#include "Abilities/SFAbilitySystemComponent.h"
#include "Abilities/SFAttributeSet.h"
#include "SFCharacter.h"

ASFPlayerState::ASFPlayerState()
{
	// Create ability system component, and set it to be explicitly replicated
	AbilitySystem = CreateDefaultSubobject<USFAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystem->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSet = CreateDefaultSubobject<USFAttributeSet>(TEXT("AttributeSet"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	// 100 is probably way too high for a shipping game, you can adjust to fit your needs.
	NetUpdateFrequency = 100.0f;
}

UAbilitySystemComponent* ASFPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

USFAttributeSet* ASFPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void ASFPlayerState::BeginPlay()
{
	Super::BeginPlay();

	HealthChangedDelegateHandle = AbilitySystem->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &ASFPlayerState::HealthChanged);
}	

void ASFPlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;
	
	ASFCharacter* Character = Cast<ASFCharacter>(GetPawn());
	FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Dead")));
	FGameplayTag UndyingTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Undying")));
	if (Character && AttributeSet->GetHealth() <= 0.0f && !AbilitySystem->HasMatchingGameplayTag(DeadTag) && !AbilitySystem->HasMatchingGameplayTag(UndyingTag))
	{
		Character->Die();
	}
}