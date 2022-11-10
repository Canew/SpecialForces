// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Abilities/SFGameplayAbility.h"
#include "SFWeapon.generated.h"

UCLASS()
class SPECIALFORCES_API ASFWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASFWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool IsEnableAutoAttack();
	virtual float GetAutoAttackInterval();

	virtual void DoAttack() {};

	TSubclassOf<class USFGameplayAbility> GetPrimaryAbility()
	{
		return PrimaryAbility;
	}
	TSubclassOf<class USFGameplayAbility> GetSecondaryAbility()
	{
		return SecondaryAbility;
	}

	virtual void SetOwnerCharacter(class ASFCharacter* Character);

	virtual void StartWeaponRecoil();
	virtual void TickWeaponRecoil();
	virtual void TickWeaponRecoilRecovery();

	virtual float GetVerticalRecoilMagnitude() const;
	virtual float GetHorizontalRecoilMagnitude() const;
	virtual bool IsRecoiling() const;
	virtual bool IsRecoilRecovering() const;;

protected:
	class ASFCharacter* OwnerCharacter;

protected:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class USFGameplayAbility> PrimaryAbility;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class USFGameplayAbility> SecondaryAbility;
	
	UPROPERTY(EditDefaultsOnly)
	bool bEnableAutoAttack = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoAttackInterval;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UCameraShakeBase> AttackCameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float VerticalRecoilMagnitude;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float HorizontalRecoilMagnitude;

	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilTime;

	FTimerHandle RecoilTimerHandle;
	bool bIsRecoiling;

	FTimerHandle RecoilRecoveryTimerHandle;
	bool bIsRecoilRecovering;
};
