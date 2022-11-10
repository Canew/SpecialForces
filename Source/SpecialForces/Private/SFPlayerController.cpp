// Fill out your copyright notice in the Description page of Project Settings.


#include "SFPlayerController.h"
#include "SFCharacter.h"

ASFPlayerController::ASFPlayerController()
{
	SetGenericTeamId(FGenericTeamId(0));
}

void ASFPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

}

void ASFPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamId)
{
	if (TeamId != NewTeamId)
	{
		TeamId = NewTeamId;
	}
}

FGenericTeamId ASFPlayerController::GetGenericTeamId() const
{
	return TeamId;
}
