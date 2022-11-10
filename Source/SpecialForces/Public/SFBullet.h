// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SFBullet.generated.h"

UCLASS()
class SPECIALFORCES_API ASFBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASFBullet();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual class UProjectileMovementComponent* GetBulletMovement() const;

protected:
	UFUNCTION()
	virtual void OnBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* BulletCollision;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* BulletMesh;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* BulletMovement;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* BulletMarkMaterial;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* BulletHitParticleDefault;

	UPROPERTY(EditDefaultsOnly)
	class UParticleSystem* BulletHitParticleBody;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameplayEffect> BulletDamageEffect;

};
