// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpecialForces/SpecialForces.h"
#include "GameFramework/Character.h"
#include "Components/TimeLineComponent.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayTagContainer.h"

#include "SFCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterHitReactDelegate, ESFDirection, Direction);

UCLASS()
class SPECIALFORCES_API ASFCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASFCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	// Called when controller possess pawn
	virtual void PossessedBy(AController* NewController) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void MoveForward(float AxisValue);
	virtual void MoveRight(float AxisValue);
	virtual void Turn(float AxisValue);
	virtual void LookUp(float AxisValue);

public:
	virtual bool IsDead() const;
	virtual void Die();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Character Death")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Character Death")
	bool bIsDieWithRagdoll;

	UPROPERTY(EditDefaultsOnly, Category = "Character Death")
	float DestroyTime = 5.0f;
	FTimerHandle DestroyTimerHandle;

public:
	virtual class USpringArmComponent* GetCharacterSpringArm() const;
	virtual class UCameraComponent* GetCharacterCamera() const;

protected:
	virtual void AttachCamera();

protected:
	float OriginTargetArmLength;
	FVector OriginTargetArmLocation;

public:
	// IAbilitySystemInterface
	/** Returns the ability system component to use for this actor. It may live on another actor, such as a Pawn using the PlayerState's component */
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	virtual void InitializeAttributes();

	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();

	void BindASCInput();

	TArray<TSubclassOf<class USFGameplayAbility>> GetCharacterAbilities();

	/* Setters for Attributes. Only use these in special cases like Respawning, otherwise use a GE to change Attributes. */
	/* These change the Attribute's Base Value. */
	
	virtual void SetHealth(float Health);

protected:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* CharacterSpringArm;
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* CharacterCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", Meta = (AllowPrivateAccess = true))
	class USFAbilitySystemComponent* AbilitySystem;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Abilities", Meta = (AllowPrivateAccess = true))
	class USFAttributeSet* AttributeSet;

	// Default abilities for this Character. These will be removed on Character death and regiven if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class USFGameplayAbility>> CharacterAbilities;

	// Default attributes for a character for initializing on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

public:
	virtual void EquipInitWeapon();
	virtual class ASFWeapon* GetEquipWeapon();

	virtual float GetPrimaryAttackRange();

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	class ASFWeapon* EquipWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", Meta = (AllowPrivateAccess = true))
	TSubclassOf<class ASFWeapon> InitWeaponClass;

	UPROPERTY(EditDefaultsOnly)
	float PrimaryAttackRange;

	// Client only
	virtual void OnRep_PlayerState() override;

public:
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	virtual void PlayHitReact(ESFDirection HitReactDirection, AActor* SourceActor);
	virtual void PlayHitReact_Implementation(ESFDirection HitReactDirection, AActor* SourceActor);
	virtual bool PlayHitReact_Validate(ESFDirection HitReactDirection, AActor* SourceActor);

	virtual ESFDirection GetHitReactDirection(FVector HitLocation);

	UFUNCTION(BlueprintNativeEvent)
	float GetDamageMultiplierByBodyParts(FName BoneName);

public:
	// Set the Hit React direction in the Animation Blueprint
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FCharacterHitReactDelegate ShowHitReact;

protected:
	FGameplayTag HitDirectionFrontTag;
	FGameplayTag HitDirectionBackTag;
	FGameplayTag HitDirectionRightTag;
	FGameplayTag HitDirectionLeftTag;
	
public:
	virtual FGenericTeamId GetGenericTeamId() const;

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	class UAIPerceptionStimuliSourceComponent* AIPerceptionStimuliSource;
	};
