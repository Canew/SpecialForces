// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "SFDamageTextWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class SPECIALFORCES_API USFDamageTextWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	USFDamageTextWidgetComponent();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(float Damage);

	void SetTargetActor(AActor* NewTargetActor);
	void SetDamageTextColor(FColor NewDamageTextColor);

protected:
	class AActor* TargetActor;

	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	FColor DamageTextColor = FColor::White;
};
