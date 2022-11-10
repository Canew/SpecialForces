// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "SFPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API ASFPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	ASFPlayerController();

	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	// Implementation IGenericTeamAgentInterface
public:
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID);

	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Team")
	FGenericTeamId TeamId;
};
