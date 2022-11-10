// Fill out your copyright notice in the Description page of Project Settings.


#include "SFWeapon.h"
#include "SFCharacter.h"
#include "Camera/CameraShakeBase.h"

// Sets default values
ASFWeapon::ASFWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	WeaponMesh->SetCollisionProfileName(TEXT("NoCollision"));
	RootComponent = WeaponMesh;

	// Initialize recoil properties
	VerticalRecoilMagnitude = 0.0f;
	HorizontalRecoilMagnitude = 0.0f;
	RecoilTime = 0.0f;
}

// Called when the game starts or when spawned
void ASFWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASFWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ASFWeapon::IsEnableAutoAttack()
{
	return bEnableAutoAttack;
}

float ASFWeapon::GetAutoAttackInterval()
{
	return AutoAttackInterval;
}

void ASFWeapon::SetOwnerCharacter(ASFCharacter* Character)
{
	OwnerCharacter = Character;
}


void ASFWeapon::StartWeaponRecoil()
{
	bIsRecoiling = true;
	GetWorldTimerManager().SetTimer(RecoilTimerHandle, FTimerDelegate::CreateLambda(
		[this]() -> void
		{
			bIsRecoiling = false;
			bIsRecoilRecovering = true;
			GetWorldTimerManager().SetTimer(RecoilRecoveryTimerHandle, FTimerDelegate::CreateLambda(
				[this]() -> void
				{
					bIsRecoilRecovering = false;
				}
			), RecoilTime, false);
		}
	), RecoilTime, false);
}

void ASFWeapon::TickWeaponRecoil()
{
	if (OwnerCharacter)
	{
		OwnerCharacter->AddControllerPitchInput(-VerticalRecoilMagnitude);
		OwnerCharacter->AddControllerYawInput(FMath::RandRange(-HorizontalRecoilMagnitude, HorizontalRecoilMagnitude));
	}
}

void ASFWeapon::TickWeaponRecoilRecovery()
{
	if (OwnerCharacter)
	{
		OwnerCharacter->AddControllerPitchInput(VerticalRecoilMagnitude/ 2.0f);
	}
}

float ASFWeapon::GetVerticalRecoilMagnitude() const
{
	return VerticalRecoilMagnitude;
}

float ASFWeapon::GetHorizontalRecoilMagnitude() const
{
	return HorizontalRecoilMagnitude;
}

bool ASFWeapon::IsRecoiling() const
{
	return bIsRecoiling;
}

bool ASFWeapon::IsRecoilRecovering() const
{
	return bIsRecoilRecovering;
}