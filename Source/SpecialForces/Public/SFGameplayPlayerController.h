// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SFPlayerController.h"
#include "SFGameplayPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API ASFGameplayPlayerController : public ASFPlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupInputComponent() override;

public:
	UFUNCTION(Client, Reliable, WithValidation)
	void ShowDamageText(float DamageAmount, class ASFCharacter* TargetCharacter, FVector HitLocation = FVector::ZeroVector, FColor DamageTextColor = FColor::White);
	void ShowDamageText_Implementation(float DamageAmount, class ASFCharacter* TargetCharacter, FVector HitLocation = FVector::ZeroVector, FColor DamageTextColor = FColor::White);
	bool ShowDamageText_Validate(float DamageAmount, class ASFCharacter* TargetCharacter, FVector HitLocation = FVector::ZeroVector, FColor DamageTextColor = FColor::White);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class USFDamageTextWidgetComponent> DamageTextClass;

protected:
	virtual void CreateCrosshairWidget();

	virtual void MoveForward(float AxisValue);
	virtual void MoveRight(float AxisValue);
	virtual void Turn(float AxisValue);
	virtual void LookUp(float AxisValue);

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> CrosshairWidgetClass;
	class UUserWidget* CrosshairWidget;
};
