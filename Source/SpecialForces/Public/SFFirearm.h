// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SFWeapon.h"
#include "SFFirearm.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API ASFFirearm : public ASFWeapon
{
	GENERATED_BODY()

public:
	ASFFirearm();

	virtual void DoAttack() override;

protected:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* Muzzle;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly)
	class USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ASFBullet> BulletClass;

	UPROPERTY(EditDefaultsOnly)
	float MaxRange;
};
