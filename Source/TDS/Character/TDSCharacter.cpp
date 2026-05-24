// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "TDS/Game/TDSGameInstance.h"
#include "TDS/Weapons/Projectiles/ProjectileDefault.h"
#include "Net/UnrealNetwork.h"


ATDSCharacter::ATDSCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	InventoryComponent = CreateDefaultSubobject<UTDSInventoryComponent>(TEXT("InventoryComponent"));
	HealthComponent = CreateDefaultSubobject<UTDSCharacterHealthComponent>(TEXT("HealthComponent"));
	EffectComponent = CreateDefaultSubobject<UTDS_EffectComponent>(TEXT("EffectComponent"));

	if (HealthComponent)
	{
		HealthComponent->OnDead.AddDynamic(this, &ATDSCharacter::CharacterDead);
	}

	if (InventoryComponent)
	{
		InventoryComponent->OnSwitchWeapon.AddDynamic(this, &ATDSCharacter::InitWeapon);
	}
	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	bReplicates = true;
}

void ATDSCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void ATDSCharacter::BeginPlay()
{
	Super::BeginPlay();
		
	EffectComponent->SetAttachTarget(GetMesh());
}

EMovementState ATDSCharacter::GetMovementState()
{
	return MovementState;
}

TArray<UTDS_StateEffect*> ATDSCharacter::GetCurrentEffectsOnChar()
{
	return EffectComponent->GetActiveEffects();
}

int32 ATDSCharacter::GetCurrentWeaponIndex()
{
	return CurrentIndexWeapon;
}

bool ATDSCharacter::GetIsAlive()
{
	bool Result = false;
	if (HealthComponent)
	{
		Result = HealthComponent->GetIsAlive();
	}
	return Result;
}

void ATDSCharacter::AttackCharEvent(bool bIsFiring)
{
	AWeaponDefault* Weapon = GetCurrentWeapon();
	
	if (!Weapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATDSCharacter::AttackCharEvent - CurrentWeapon -NULL"));
		return;
	}
	
	Weapon->SetWeaponStateFire_OnServer(bIsFiring);
}

void ATDSCharacter::RequestSwitchWeaponByIndex(int32 Id)
{
	TrySwitchWeaponToIndexByKeyInput_OnServer(Id);
}

void ATDSCharacter::SetAimLocation(FVector Location)
{
	if (!CurrentWeapon)
		return;
	
	FVector Displacement = FVector::ZeroVector;
	bool bIsReduceDispersion = false;
	
	if (MovementState == EMovementState::Aim_State || MovementState == EMovementState::AimWalk_State)
	{
		Displacement = FVector(0.0f, 0.0f, 160.0f);
		bIsReduceDispersion = true;
	}
	else if (MovementState == EMovementState::Walk_State || MovementState == EMovementState::Run_State)
	{
		Displacement = FVector(0.0f, 0.0f, 120.0f);
	}
			
	CurrentWeapon->UpdateWeaponByCharacterMovementState_OnServer(Location + Displacement, bIsReduceDispersion);
	
}

void ATDSCharacter::SetFiring(bool bIsFiring)
{
	if (HealthComponent && HealthComponent->GetIsAlive())
	{
		AttackCharEvent(bIsFiring);
	}
}

void ATDSCharacter::RequestReload()
{
	TryReloadWeapon();
}

void ATDSCharacter::RequestAbility()
{
	TryAbilityEnabled_OnServer();
}

void ATDSCharacter::RequestDropWeapon()
{
	DropCurrentWeapon();
}

void ATDSCharacter::RequestSwitchNextWeapon()
{
	TrySwitchNextWeapon();
}

void ATDSCharacter::RequestSwitchPreviousWeapon()
{
	TrySwitchPreviousWeapon();
}

void ATDSCharacter::CharacterUpdate()
{
	float ResSpeed = 600.0f;
	
	switch (MovementState)
	{
	case EMovementState::Aim_State:
		ResSpeed = MovementInfo.AimSpeed;
		break;
	case EMovementState::Walk_State:
		ResSpeed = MovementInfo.WalkSpeed;
		break;
	case EMovementState::Run_State:
		ResSpeed = MovementInfo.RunSpeed;
		break;
	case EMovementState::AimWalk_State:
		ResSpeed = MovementInfo.AimWalkSpeed;
		break;
	case EMovementState::Sprint_State:
		ResSpeed = MovementInfo.SprintSpeed;
		break;
	default:
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = ResSpeed;
}

AWeaponDefault* ATDSCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ATDSCharacter::InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetGameInstance());
	FWeaponInfo WeaponInfo;
	if (GI)
	{
		if (GI->GetWeaponInfoByName(IdWeaponName, WeaponInfo))
		{
			if (WeaponInfo.WeaponClass)
			{
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRotation = FRotator(0);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				AWeaponDefault* Weapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(WeaponInfo.WeaponClass, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (Weapon)
				{
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					Weapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
					CurrentWeapon = Weapon;

					Weapon->IdWeaponName = IdWeaponName;
					Weapon->WeaponSetting = WeaponInfo;

					Weapon->ReloadTime = WeaponInfo.ReloadTime;
					Weapon->UpdateStateWeapon_OnServer(MovementState);

					Weapon->AdditionalWeaponInfo = WeaponAdditionalInfo;
					CurrentIndexWeapon = NewCurrentIndexWeapon;

					Weapon->OnWeaponReloadStart.AddDynamic(this, &ATDSCharacter::WeaponReloadStart);
					Weapon->OnWeaponReloadEnd.AddDynamic(this, &ATDSCharacter::WeaponReloadEnd);
					Weapon->OnWeaponFire.AddDynamic(this, &ATDSCharacter::WeaponFire);


					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
					{
						CurrentWeapon->InitReload();
					}
					if (InventoryComponent)
					{
						InventoryComponent->OnWeaponAmmoAvailable.Broadcast(Weapon->WeaponSetting.WeaponType);
					}
				}
			}
		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("ATDSCharacter::InitWeapon - Weapon not found in table -NULL"));
		}
		
	}
}


void ATDSCharacter::TryReloadWeapon()
{
	if (HealthComponent && HealthComponent->GetIsAlive() && CurrentWeapon && !CurrentWeapon->WeaponReloading)
	{
		TryReloadWeapon_OnServer();
	}
}

void ATDSCharacter::WeaponReloadStart(UAnimMontage* Anim)
{
	WeaponReloadStart_BP(Anim);
}

void ATDSCharacter::WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->AmmoSlotChangeValue(CurrentWeapon->WeaponSetting.WeaponType, AmmoTake);
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
	}

	WeaponReloadEnd_BP(bIsSuccess);
}

void ATDSCharacter::TrySwitchWeaponToIndexByKeyInput_OnServer_Implementation(int32 ToIndex)
{
	if (!CurrentWeapon || !InventoryComponent->WeaponSlots.IsValidIndex(ToIndex))
		return;
	
	if (CurrentIndexWeapon == ToIndex)
		return;
	
	int32 OldIndex = CurrentIndexWeapon;

	FAdditionalWeaponInfo OldInfo = CurrentWeapon->AdditionalWeaponInfo;
	
	if (CurrentWeapon->WeaponReloading)
		CurrentWeapon->CancelReload();
	
	InventoryComponent->SwitchWeaponByIndex(ToIndex, OldIndex, OldInfo);
}

void ATDSCharacter::DropCurrentWeapon()
{
	if (InventoryComponent)
	{
		InventoryComponent->DropWeaponByIndex_OnServer(CurrentIndexWeapon);
	}
}

void ATDSCharacter::WeaponFire(UAnimMontage* Anim)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
	}

	WeaponFire_BP(Anim);
}

void ATDSCharacter::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
	//BP
}

void ATDSCharacter::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
{
	//BP
}

void ATDSCharacter::WeaponFire_BP_Implementation(UAnimMontage* Anim)
{
	//BP
}

void ATDSCharacter::TrySwitchNextWeapon()
{
	if (!CurrentWeapon || !InventoryComponent || InventoryComponent->WeaponSlots.Num() <= 1)
		return;
	
	int8 OldIndex = CurrentIndexWeapon;

	FAdditionalWeaponInfo OldInfo = CurrentWeapon->AdditionalWeaponInfo;
	
	if (CurrentWeapon->WeaponReloading)
		CurrentWeapon->CancelReload();
	
	InventoryComponent->SwitchWeaponToIndexByNextPreviousIndex(CurrentIndexWeapon + 1, OldIndex, OldInfo, true);
}

void ATDSCharacter::TrySwitchPreviousWeapon()
{
	if (!CurrentWeapon || !InventoryComponent || InventoryComponent->WeaponSlots.Num() <= 1)
		return;
	
	int8 OldIndex = CurrentIndexWeapon;

	FAdditionalWeaponInfo OldInfo = CurrentWeapon->AdditionalWeaponInfo;
	
	if (CurrentWeapon->WeaponReloading)
		CurrentWeapon->CancelReload();
	
	InventoryComponent->SwitchWeaponToIndexByNextPreviousIndex(CurrentIndexWeapon - 1, OldIndex, OldInfo, false);
	
}

void ATDSCharacter::TryAbilityEnabled()
{
	TryAbilityEnabled_OnServer();
}

void ATDSCharacter::TryAbilityEnabled_OnServer_Implementation()
{
	if (!AbilityEffect)
		return;
	
	UTDS_StateEffect* NewEffect = NewObject<UTDS_StateEffect>(this, AbilityEffect);
	if (NewEffect)
		NewEffect->InitObject(this, NAME_None);
	
}

EPhysicalSurface ATDSCharacter::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;
	if (HealthComponent)
	{
		if (HealthComponent->GetCurrentShield() <= 0)
		{
			if (GetMesh())
			{
				UMaterialInterface* Material = GetMesh()->GetMaterial(0);
				if (Material)
				{
					Result = Material->GetPhysicalMaterial()->SurfaceType;
				}
			}
		}
	}

	return Result;
}

TArray<UTDS_StateEffect*> ATDSCharacter::GetAllCurrentEffects_Implementation()
{
	return EffectComponent->GetActiveEffects();
}

void ATDSCharacter::RemoveEffect_Implementation(UTDS_StateEffect* RemoveEffect)
{
	EffectComponent->RemoveEffect(RemoveEffect);
}

void ATDSCharacter::AddEffect_Implementation(UTDS_StateEffect* NewEffect)
{
	EffectComponent->ApplyEffect(NewEffect);
}

void ATDSCharacter::TryReloadWeapon_OnServer_Implementation()
{	
	if (!CurrentWeapon)
		return;
	
	if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound && CurrentWeapon->CheckCanWeaponReload())
		CurrentWeapon->InitReload();
}

void ATDSCharacter::SetActorRotationByYaw_OnServer_Implementation(float Yaw)
{
	SetActorRotationByYaw_Multicast(Yaw);
}

void ATDSCharacter::SetActorRotationByYaw_Multicast_Implementation(float Yaw)
{
	if (Controller && !Controller->IsLocalPlayerController())
		SetActorRotation(FQuat(FRotator(0.0f, Yaw, 0.0f)));
}

void ATDSCharacter::SetMovementState_OnServer_Implementation(EMovementState NewState)
{
	SetMovementState_Multicast(NewState);
}

void ATDSCharacter::SetMovementState_Multicast_Implementation(EMovementState NewState)
{
	MovementState = NewState;
	CharacterUpdate();
	
	AWeaponDefault* Weapon = GetCurrentWeapon();
	if (Weapon)
		Weapon->UpdateStateWeapon_OnServer(MovementState);
}

void ATDSCharacter::PlayAnim_Multicast_Implementation(UAnimMontage* Anim)
{
	if (GetMesh() && GetMesh()->GetAnimInstance())
		GetMesh()->GetAnimInstance()->Montage_Play(Anim);
}

void ATDSCharacter::CharacterDead_BP_Implementation()
{
	//BP
}

void ATDSCharacter::CharacterDead()
{
	CharacterDead_BP();

	if (HasAuthority())
	{
		float TimeAnim = 0.0f;
		int32 rnd = FMath::RandHelper(DeadsAnim.Num());
		if (DeadsAnim.IsValidIndex(rnd) && DeadsAnim[rnd] && GetMesh() && GetMesh()->GetAnimInstance())
		{
			TimeAnim = DeadsAnim[rnd]->GetPlayLength();
			PlayAnim_Multicast(DeadsAnim[rnd]);
		}

		if (GetController())
		{
			GetController()->UnPossess();
		}

		float DecreaseAnimTimer = FMath::FRandRange(0.2f, 1.0f);

		GetWorldTimerManager().SetTimer(TimerHandle_RagDollTimer, this, &ATDSCharacter::EnableRagdoll_Multicast, TimeAnim - DecreaseAnimTimer, false);

		SetLifeSpan(20.0f);
		if (GetCurrentWeapon())
			GetCurrentWeapon()->SetLifeSpan(20.0f);
	}
	else
	{
		AttackCharEvent(false);
	}

	if (GetCapsuleComponent())
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void ATDSCharacter::EnableRagdoll_Multicast_Implementation()
{
	if (GetMesh())
	{
		GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
		GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
}

float ATDSCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HealthComponent && HealthComponent->GetIsAlive())
	{
		HealthComponent->ChangeHealthValue_OnServer(-DamageAmount);
	}

	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		AProjectileDefault* myProjectile = Cast<AProjectileDefault>(DamageCauser);
		if (myProjectile)
		{
			UTypes::AddEffectBySurfaceType(this, NAME_None,myProjectile->ProjectileSetting.Effect, GetSurfaceType());
		}
	}

	return ActualDamage;
}


void ATDSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDSCharacter, MovementState);
	DOREPLIFETIME(ATDSCharacter, CurrentWeapon);
	DOREPLIFETIME(ATDSCharacter, CurrentIndexWeapon);
}


bool ATDSCharacter::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool Wrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	Wrote |= EffectComponent->ReplicateEffectSubobjects(Channel, Bunch, RepFlags); 
	return Wrote;
}
