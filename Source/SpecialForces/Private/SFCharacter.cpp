// Fill out your copyright notice in the Description page of Project Settings.


#include "SFCharacter.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Abilities/SFAbilitySystemComponent.h"
#include "Abilities/SFAttributeSet.h"
#include "Abilities/SFGameplayAbility.h"
#include "Components/SFCharacterMovementComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Curves/CurveFloat.h"
#include "SFWeapon.h"
#include "SFPlayerState.h"
#include "LevelEditor.h"

// Sets default values
ASFCharacter::ASFCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USFCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bAlwaysRelevant = true;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("SFCharacter"));

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetGenerateOverlapEvents(true);

	// Set controller properties.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set Character Movement Component.
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	OriginTargetArmLength = 250.0f;
	OriginTargetArmLocation = FVector(0.0f, 30.0f, 100.0f);

	HitDirectionFrontTag = FGameplayTag::RequestGameplayTag(TEXT("Character.HitReact.Front"));
	HitDirectionBackTag = FGameplayTag::RequestGameplayTag(TEXT("Character.HitReact.Front"));
	HitDirectionRightTag = FGameplayTag::RequestGameplayTag(TEXT("Character.HitReact.Front"));
	HitDirectionLeftTag = FGameplayTag::RequestGameplayTag(TEXT("Character.HitReact.Front"));

	bIsDieWithRagdoll = false;

	AIPerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("AIPerceptionStimuliSource"));

	PrimaryAttackRange = 250.0f;
}

// Called when the game starts or when spawned-
void ASFCharacter::BeginPlay()
{
	Super::BeginPlay();

	AIPerceptionStimuliSource->RegisterForSense(UAISenseConfig_Sight::StaticClass());
}

void ASFCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void ASFCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (NewController->IsPlayerController())
	{
		AttachCamera();
	}

	ASFPlayerState* PS = GetPlayerState<ASFPlayerState>();
	if (PS)
	{
		// Set the ASC on the Server. Clients do this in OnRep_PlayerState()
		AbilitySystem = Cast<USFAbilitySystemComponent>(PS->GetAbilitySystemComponent());
		AbilitySystem->InitAbilityActorInfo(PS, this);

		AbilitySystem->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Undying"))));

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSet = PS->GetAttributeSet();
		InitializeAttributes();
		SetHealth(AttributeSet->GetMaxHealth());

		AddCharacterAbilities();

		EquipInitWeapon();

		AbilitySystem->SetTagMapCount(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Dead"))), 0);
		AbilitySystem->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Undying"))));
	}
}

// Called every frame
void ASFCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EquipWeapon)
	{
		if (EquipWeapon->IsRecoiling())
		{
			EquipWeapon->TickWeaponRecoil();
		}
		else if (EquipWeapon->IsRecoilRecovering())
		{
			EquipWeapon->TickWeaponRecoilRecovery();
		}
	}
}

// Called to bind functionality to input
void ASFCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	BindASCInput();
}

void ASFCharacter::MoveForward(float AxisValue)
{
	FVector Direction = AxisValue * FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X);
	Direction.Z = 0.0f;
	Direction.Normalize();
	AddMovementInput(Direction);
}

void ASFCharacter::MoveRight(float AxisValue)
{
	FVector Direction = AxisValue * FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y);
	Direction.Z = 0.0f;
	Direction.Normalize();
	AddMovementInput(Direction);
}

void ASFCharacter::Turn(float AxisValue)
{
	AddControllerYawInput(AxisValue);
}

void ASFCharacter::LookUp(float AxisValue)
{
	AddControllerPitchInput(AxisValue);
}

bool ASFCharacter::IsDead() const
{
	return AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Dead"))));
}

void ASFCharacter::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0.0f;
	GetCharacterMovement()->Velocity = FVector::ZeroVector;
	GetCharacterMovement()->bUseRVOAvoidance = false;

	if (AbilitySystem)
	{
		AbilitySystem->CancelAllAbilities();

		AbilitySystem->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Dead"))));
	}

	if (bIsDieWithRagdoll)
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("NoCollision"));
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);
	}
	else if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	
	GetWorldTimerManager().SetTimer(DestroyTimerHandle, FTimerDelegate::CreateLambda(
		[this]() -> void
		{
			if (EquipWeapon)
			{
				EquipWeapon->Destroy();
			}
			AController* OldController = GetController();
			DetachFromControllerPendingDestroy();
			if (OldController)
			{
				OldController->Destroy();
			}
			Destroy();
		}), DestroyTime, false, DestroyTime);
}

UAbilitySystemComponent* ASFCharacter::GetAbilitySystemComponent() const
{
	ASFPlayerState* PS = GetPlayerState<ASFPlayerState>();
	if (PS)
	{
		return PS->GetAbilitySystemComponent();
	}
	else
	{
		return nullptr;
	}
}

void ASFCharacter::InitializeAttributes()
{
	if (!AbilitySystem)
	{
		return;
	}

	if (!DefaultAttributes)
	{
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystem->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystem->MakeOutgoingSpec(DefaultAttributes, 1, EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystem->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystem);
	}
}

void ASFCharacter::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !AbilitySystem)
	{
		return;
	}

	for (TSubclassOf<USFGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystem->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}
}

void ASFCharacter::BindASCInput()
{
	if (AbilitySystem && IsValid(InputComponent))
	{
		AbilitySystem->BindAbilityActivationToInputComponent(InputComponent, FGameplayAbilityInputBinds(FString("ConfirmTarget"),
			FString("CancelTarget"), FString("ESFAbilityInputID"), static_cast<int32>(ESFAbilityInputID::Confirm), static_cast<int32>(ESFAbilityInputID::Cancel)));
	}
}

TArray<TSubclassOf<class USFGameplayAbility>> ASFCharacter::GetCharacterAbilities()
{
	return CharacterAbilities;
}

void ASFCharacter::SetHealth(float Health)
{
	if (AttributeSet)
	{
		AttributeSet->SetHealth(Health);
	}
}

USpringArmComponent* ASFCharacter::GetCharacterSpringArm() const
{
	return CharacterSpringArm;
}

UCameraComponent* ASFCharacter::GetCharacterCamera() const
{
	return CharacterCamera;
}

void ASFCharacter::AttachCamera()
{
	CharacterSpringArm = NewObject<USpringArmComponent>(this, USpringArmComponent::StaticClass(), TEXT("CharacterSpringArm"));
	if (CharacterSpringArm)
	{
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);
		CharacterSpringArm->RegisterComponent();
		CharacterSpringArm->bUsePawnControlRotation = true;
		CharacterSpringArm->TargetArmLength = OriginTargetArmLength;
		CharacterSpringArm->SetRelativeLocation(OriginTargetArmLocation);
		CharacterSpringArm->bEnableCameraLag = true;
		CharacterSpringArm->CameraLagSpeed = 15.0f;
		CharacterSpringArm->AttachToComponent(RootComponent, AttachmentTransformRules);
		CharacterSpringArm->CreationMethod = EComponentCreationMethod::Instance;
	}

	CharacterCamera = NewObject<UCameraComponent>(this, UCameraComponent::StaticClass(), TEXT("CharacterCamera"));
	if (CharacterCamera)
	{
		FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);
		CharacterCamera->RegisterComponent();
		CharacterCamera->AttachToComponent(CharacterSpringArm, AttachmentTransformRules);
		CharacterCamera->CreationMethod = EComponentCreationMethod::Instance;
	}
}

void ASFCharacter::EquipInitWeapon()
{
	if (InitWeaponClass)
	{
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = this;
		EquipWeapon = GetWorld()->SpawnActor<ASFWeapon>(InitWeaponClass, ActorSpawnParameters);
		if (EquipWeapon)
		{
			EquipWeapon->SetInstigator(this);
			EquipWeapon->SetOwnerCharacter(this);

			FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);
			EquipWeapon->AttachToComponent(GetMesh(), AttachmentTransformRules, TEXT("hand_r_weapon"));

			TSubclassOf<USFGameplayAbility> PrimaryAbilityClass = EquipWeapon->GetPrimaryAbility();
			if (PrimaryAbilityClass)
			{
				AbilitySystem->GiveAbility(FGameplayAbilitySpec(PrimaryAbilityClass, 1, static_cast<int32>(PrimaryAbilityClass.GetDefaultObject()->AbilityInputID), this));
			}
			TSubclassOf<USFGameplayAbility> SecondaryAbilityClass = EquipWeapon->GetSecondaryAbility();
			if (SecondaryAbilityClass)
			{
				AbilitySystem->GiveAbility(FGameplayAbilitySpec(SecondaryAbilityClass, 1, static_cast<int32>(SecondaryAbilityClass.GetDefaultObject()->AbilityInputID), this));
			}
		}
	}
}

ASFWeapon* ASFCharacter::GetEquipWeapon()
{
	return EquipWeapon;
}

float ASFCharacter::GetPrimaryAttackRange()
{
	return PrimaryAttackRange;
}

void ASFCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ASFPlayerState* PS = GetPlayerState<ASFPlayerState>();
	if (PS)
	{
		// Set the ASC for clients. Server does this in PossessedBy.
		AbilitySystem = Cast<USFAbilitySystemComponent>(PS->GetAbilitySystemComponent());

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		AbilitySystem->InitAbilityActorInfo(PS, this);

		// Bind player input to the AbilitySystemComponent. Also called in SetupPlayerInputComponent because of a potential race condition.
		BindASCInput();

		// Set the AttributeSetBase for convenience attribute functions
		AttributeSet = PS->GetAttributeSet();

		InitializeAttributes();

		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(AttributeSet->GetMaxHealth());

		// Forcibly set the DeadTag count to 0
		AbilitySystem->SetTagMapCount(FGameplayTag::RequestGameplayTag(FName(TEXT("Character.State.Dead"))), 0);
	}
}

ESFDirection ASFCharacter::GetHitReactDirection(FVector HitLocation)
{
	const FVector& ActorLocation = GetActorLocation();
	// PointPlaneDist is super cheap - 1 vector subtraction, 1 dot product.
	float DistanceToFrontBackPlane = FVector::PointPlaneDist(HitLocation, ActorLocation, GetActorRightVector());
	float DistanceToRightLeftPlane = FVector::PointPlaneDist(HitLocation, ActorLocation, GetActorForwardVector());

	if (FMath::Abs(DistanceToFrontBackPlane) <= FMath::Abs(DistanceToRightLeftPlane))
	{
		// Determine if Front or Back

		// Can see if it's left or right of Left/Right plane which would determine Front or Back
		if (DistanceToRightLeftPlane >= 0)
		{
			return ESFDirection::Front;
		}
		else
		{
			return ESFDirection::Back;
		}
	}
	else
	{
		// Determine if Right or Left

		if (DistanceToFrontBackPlane >= 0)
		{
			return ESFDirection::Right;
		}
		else
		{
			return ESFDirection::Left;
		}
	}

	return ESFDirection::Front;
}

FGenericTeamId ASFCharacter::GetGenericTeamId() const
{
	IGenericTeamAgentInterface* GenericTeamAgentInterface = Cast< IGenericTeamAgentInterface>(GetController());
	if (GenericTeamAgentInterface)
	{
		return GenericTeamAgentInterface->GetGenericTeamId();
	}
	else
	{
		return FGenericTeamId::NoTeam;
	}
	}

float ASFCharacter::GetDamageMultiplierByBodyParts_Implementation(FName BoneName)
{
	return 1.0f;
}

void ASFCharacter::PlayHitReact_Implementation(ESFDirection HitReactDirection, AActor* SourceActor)
{
	if (!IsDead() && HitReactDirection != ESFDirection::None)
	{
		ShowHitReact.Broadcast(HitReactDirection);
	}
}

bool ASFCharacter::PlayHitReact_Validate(ESFDirection HitReactDirection, AActor* SourceActor)
{
	return true;
}