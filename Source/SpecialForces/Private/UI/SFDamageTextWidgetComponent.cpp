// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SFDamageTextWidgetComponent.h"

USFDamageTextWidgetComponent::USFDamageTextWidgetComponent()
{
	SetGenerateOverlapEvents(false);
}

void USFDamageTextWidgetComponent::SetTargetActor(AActor* NewTargetActor)
{
	TargetActor = NewTargetActor;
}

void USFDamageTextWidgetComponent::SetDamageTextColor(FColor NewDamageTextColor)
{
	DamageTextColor = NewDamageTextColor;
}
