// Fill out your copyright notice in the Description page of Project Settings.


#include "SFBullet.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"
#include "PhysicsEngine/BodyInstance.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "SFCharacter.h"

// Sets default values
ASFBullet::ASFBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BulletCollision = CreateDefaultSubobject<USphereComponent>(TEXT("BulletCollision"));
	BulletCollision->SetSphereRadius(4.0f);
	BulletCollision->SetCollisionProfileName(TEXT("SFBullet"));
	RootComponent = BulletCollision;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	BulletMesh->SetupAttachment(BulletCollision);

	BulletMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("BulletMovement"));
	BulletMovement->InitialSpeed = 5000.0f;
}

// Called when the game starts or when spawned
void ASFBullet::BeginPlay()
{
	Super::BeginPlay();

	BulletCollision->OnComponentBeginOverlap.AddDynamic(this, &ASFBullet::OnBeginOverlap);
}

void ASFBullet::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Called every frame
void ASFBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UProjectileMovementComponent* ASFBullet::GetBulletMovement() const
{
	return BulletMovement;
}

void ASFBullet::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator() || OtherActor == this)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UAbilitySystemComponent* Source = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator());
	UAbilitySystemComponent* Target = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);

	FVector SweepLocation = SweepResult.Location;
	if (SweepResult.Location.IsNearlyZero())
	{
		SweepLocation = OtherActor->GetActorLocation();
	}

	if (Source && Target)
	{
		if (BulletDamageEffect)
		{
			FGameplayEffectContextHandle GameplayEffectContext = Source->MakeEffectContext();
			GameplayEffectContext.AddHitResult(SweepResult, true);
			GameplayEffectContext.AddSourceObject(this);
			Source->ApplyGameplayEffectToTarget(BulletDamageEffect.GetDefaultObject(), Target, UGameplayEffect::INVALID_LEVEL, GameplayEffectContext);
		}
	}

	if (BulletMarkMaterial)
	{
		FRotator BulletMarkRotator = UKismetMathLibrary::MakeRotFromX(SweepResult.Normal);
		BulletMarkRotator.Roll = FMath::RandRange(0.0f, 360.0f);
		UDecalComponent* BulletMark = UGameplayStatics::SpawnDecalAttached(
			BulletMarkMaterial,
			FVector(5.0f, 2.0f, 2.0f),
			OtherComp,
			NAME_None,
			SweepLocation,
			BulletMarkRotator,
			EAttachLocation::KeepWorldPosition,
			5.0f);
		BulletMark->SetFadeScreenSize(0.005f);
	}

	TWeakObjectPtr<UPhysicalMaterial> HitPhysMaterial = SweepResult.PhysMaterial;
	if (HitPhysMaterial.IsValid() && HitPhysMaterial->SurfaceType.GetValue() == EPhysicalSurface::SurfaceType1)
	{
		if (BulletHitParticleBody)
		{
			FRotator BulletHitParticleRotator = UKismetMathLibrary::MakeRotFromZ(SweepResult.Normal);
			UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(
				BulletHitParticleBody,
				OtherComp,
				NAME_None,
				SweepLocation,
				BulletHitParticleRotator,
				EAttachLocation::KeepWorldPosition);
		}
	}
	else
	{
		UPhysicalMaterial* OtherCompPhysMaterial = OtherComp->GetBodyInstance()->GetSimplePhysicalMaterial();
		if (OtherCompPhysMaterial && OtherCompPhysMaterial->SurfaceType.GetValue() == EPhysicalSurface::SurfaceType1)
		{
			if (BulletHitParticleBody)
			{
				FRotator BulletHitParticleRotator = UKismetMathLibrary::MakeRotFromZ(SweepResult.Normal);
				UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(
					BulletHitParticleBody,
					OtherComp,
					NAME_None,
					SweepLocation,
					BulletHitParticleRotator,
					EAttachLocation::KeepWorldPosition);
			}
		}
		else
		{
			if (BulletHitParticleDefault)
			{
				FRotator BulletHitParticleRotator = UKismetMathLibrary::MakeRotFromZ(SweepResult.Normal);
				UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(
					BulletHitParticleDefault,
					OtherComp,
					NAME_None,
					SweepLocation,
					BulletHitParticleRotator,
					EAttachLocation::KeepWorldPosition);
			}
		}
	}

	Destroy();
}
	