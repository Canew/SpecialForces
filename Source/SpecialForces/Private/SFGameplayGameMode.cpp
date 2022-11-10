// Fill out your copyright notice in the Description page of Project Settings.


#include "SFGameplayGameMode.h"
#include "SFCharacter.h"
#include "SFGameplayPlayerController.h"

ASFGameplayGameMode::ASFGameplayGameMode()
{
	DefaultPawnClass = ASFCharacter::StaticClass();
	PlayerControllerClass = ASFGameplayPlayerController::StaticClass();
}
