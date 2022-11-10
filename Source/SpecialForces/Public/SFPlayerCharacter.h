// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SFCharacter.h"
#include "SFPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API ASFPlayerCharacter : public ASFCharacter
{
	GENERATED_BODY()

public:
	ASFPlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;


};
