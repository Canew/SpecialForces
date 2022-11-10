// Fill out your copyright notice in the Description page of Project Settings.


#include "SFFirearm.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AISense_Hearing.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SFBullet.h"
#include "SFCharacter.h"

ASFFirearm::ASFFirearm()
{
	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(WeaponMesh, TEXT("weapon_muzzle"));

	MaxRange = 10000.0f;
}

void ASFFirearm::DoAttack()
{
	Super::DoAttack();

	UWorld* World = GetWorld();
	if (!World || !OwnerCharacter)
	{
		return;
	}

	if (BulletClass)
	{
		FTransform SpawnTransform = FTransform(FRotator::ZeroRotator, Muzzle->GetComponentLocation());
		ASFBullet* Bullet = World->SpawnActorDeferred<ASFBullet>(BulletClass, SpawnTransform, this, GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Bullet->SetInstigator(GetInstigator());
		Bullet->SetLifeSpan(MaxRange / Bullet->GetBulletMovement()->InitialSpeed);
		

		FHitResult HitResult;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);
		CollisionQueryParams.AddIgnoredActor(GetInstigator());
		bool bHit = World->LineTraceSingleByChannel(HitResult,
			OwnerCharacter->GetCharacterCamera()->GetComponentLocation(),
			OwnerCharacter->GetCharacterCamera()->GetComponentLocation() + (MaxRange * OwnerCharacter->GetCharacterCamera()->GetForwardVector()),
			ECollisionChannel::ECC_GameTraceChannel3,
			CollisionQueryParams);

		FVector EndLocation = bHit ? HitResult.Location : OwnerCharacter->GetCharacterCamera()->GetComponentLocation() + (MaxRange * OwnerCharacter->GetCharacterCamera()->GetForwardVector());
		FVector FireDirection = EndLocation - Muzzle->GetComponentLocation();
		FireDirection.Normalize();
		Bullet->GetBulletMovement()->Velocity = FireDirection;

		Bullet->FinishSpawning(SpawnTransform);

		UAISense_Hearing::ReportNoiseEvent(GetWorld(), OwnerCharacter->GetActorLocation(), 1.5f, OwnerCharacter, 0.0f);

		StartWeaponRecoil();
	}
	else
	{
		return;
	}

	if (MuzzleEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			MuzzleEffect,
			Muzzle,
			NAME_None,
			Muzzle->GetComponentLocation(),
			Muzzle->GetComponentRotation(),
			EAttachLocation::KeepWorldPosition,
			true,
			true
		);
	}

	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			FireSound,
			Muzzle->GetComponentLocation()
		);
	}

	if (AttackCameraShake)
	{
		if (OwnerCharacter->IsPlayerControlled())
		{
			APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
			PC->ClientStartCameraShake(AttackCameraShake);
		}
	}
}
