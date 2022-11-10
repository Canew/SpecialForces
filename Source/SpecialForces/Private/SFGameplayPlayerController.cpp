// Fill out your copyright notice in the Description page of Project Settings.


#include "SFGameplayPlayerController.h"
#include "SFCharacter.h"
#include "SFPlayerCharacter.h"
#include "UI/SFDamageTextWidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/SFCharacterMovementComponent.h"
#include "Components/WidgetComponent.h"

void ASFGameplayPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreateCrosshairWidget();
}

void ASFGameplayPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis(TEXT("MoveForward"), this, &ASFGameplayPlayerController::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &ASFGameplayPlayerController::MoveRight);
	InputComponent->BindAxis(TEXT("Turn"), this, &ASFGameplayPlayerController::Turn);
	InputComponent->BindAxis(TEXT("LookUp"), this, &ASFGameplayPlayerController::LookUp);
}

void ASFGameplayPlayerController::ShowDamageText_Implementation(float DamageAmount, ASFCharacter* TargetCharacter, FVector HitLocation, FColor DamageTextColor)
{
	if (TargetCharacter && DamageTextClass)
	{
		USFDamageTextWidgetComponent* DamageText = NewObject<USFDamageTextWidgetComponent>(TargetCharacter, DamageTextClass);
		float Distance = (GetPawn()->GetActorLocation() - TargetCharacter->GetActorLocation()).Size();
		Distance = FMath::Clamp(Distance, 500.0f, MAX_FLT);
		if (DamageText)
		{
			DamageText->RegisterComponent();
			FVector DamageTextAddtiveLocation;
			if (HitLocation.IsNearlyZero())
			{
				DamageText->SetWorldLocation(TargetCharacter->GetActorLocation());
				DamageTextAddtiveLocation = Distance * FMath::RandRange(0.08f, 0.11f) * GetPawn()->GetActorRightVector() + Distance * FMath::RandRange(0.02f, 0.04f) * GetPawn()->GetActorUpVector();
			}
			else
			{
				DamageText->SetWorldLocation(HitLocation);
				DamageTextAddtiveLocation = Distance * FMath::RandRange(0.05f, 0.08f) * GetPawn()->GetActorRightVector() + Distance * FMath::RandRange(0.02f, 0.04f) * -GetPawn()->GetActorUpVector();
			}
			DamageText->SetRelativeLocation(DamageText->GetRelativeLocation() + DamageTextAddtiveLocation);
			DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
			DamageText->SetTargetActor(TargetCharacter);
			DamageText->SetDamageTextColor(DamageTextColor);
			DamageText->SetDamageText(DamageAmount);
		}
	}
}

bool ASFGameplayPlayerController::ShowDamageText_Validate(float DamageAmount, ASFCharacter* TargetCharacter, FVector HitLocation, FColor DamageTextColor)
{
	return true;
}

void ASFGameplayPlayerController::CreateCrosshairWidget()
{
	if (CrosshairWidgetClass)
	{
		CrosshairWidget = CreateWidget(this, CrosshairWidgetClass);
		CrosshairWidget->AddToViewport();
	}
}

void ASFGameplayPlayerController::MoveForward(float AxisValue)
{
	ASFCharacter* SFCharacter = Cast<ASFCharacter>(GetPawn());
	if (SFCharacter)
	{
		SFCharacter->MoveForward(AxisValue);
	}
}

void ASFGameplayPlayerController::MoveRight(float AxisValue)
{
	ASFCharacter* SFCharacter = Cast<ASFCharacter>(GetPawn());
	if (SFCharacter)
	{
		SFCharacter->MoveRight(AxisValue);
	}
}

void ASFGameplayPlayerController::Turn(float AxisValue)
{
	ASFCharacter* SFCharacter = Cast<ASFCharacter>(GetPawn());
	if (SFCharacter)
	{
		SFCharacter->Turn(AxisValue);
	}
}

void ASFGameplayPlayerController::LookUp(float AxisValue)
{
	ASFCharacter* SFCharacter = Cast<ASFCharacter>(GetPawn());
	if (SFCharacter)
	{
		SFCharacter->LookUp(AxisValue);
	}
}
