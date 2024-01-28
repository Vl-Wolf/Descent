// Copyright Epic Games, Inc. All Rights Reserved.

#include "Descent_Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Descent/Game/Descent_GameInstance.h"
#include "Descent/Weapons/Projectiles/ProjectileDefault.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/ActorChannel.h"


ADescent_Character::ADescent_Character()
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

	InventoryComponent = CreateDefaultSubobject<UDescent_InventoryComponent>(TEXT("InventoryComponent"));
	HealthComponent = CreateDefaultSubobject<UDescent_CharacterHealthComponent>(TEXT("HealthComponent"));

	if (HealthComponent)
	{
		HealthComponent->OnDead.AddDynamic(this, &ADescent_Character::CharacterDead);
	}

	if (InventoryComponent)
	{
		InventoryComponent->OnSwitchWeapon.AddDynamic(this, &ADescent_Character::InitWeapon);
	}

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//Network
	bReplicates = true;
}

void ADescent_Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);


	if (CurrentCursor)
	{
		APlayerController* myPC = Cast<APlayerController>(GetController());
		if (myPC && myPC->IsLocalPlayerController())
		{
			FHitResult TraceHitResult;
			myPC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();

			CurrentCursor->SetWorldLocation(TraceHitResult.Location);
			CurrentCursor->SetWorldRotation(CursorR);
		}
	}

	MovementTick(DeltaSeconds);
}

void ADescent_Character::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld() && GetWorld()->GetNetMode() != NM_DedicatedServer)
	{
		if (CursorMaterial && GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_Authority)
		{
			CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
		}
	}

}


void ADescent_Character::MovementTick(float DeltaTime)
{
	if (HealthComponent && HealthComponent->GetIsAlive())
	{

		if (GetController() && GetController()->IsLocalController())
		{
			AddMovementInput(FVector(1.0f, 0.0f, 0.0f), AxisX);
			AddMovementInput(FVector(0.0f, 1.0f, 0.0f), AxisY);

			/*FString SEnum = UEnum::GetValueAsString(GetMovementState());

			UE_LOG(LogTDS_Net, Warning, TEXT("Movement state - %s"), *SEnum); */ 

			if (MovementState == EMovementState::Sprint_State)
			{
				FVector myRotationVector = FVector(AxisX, AxisY, 0.0f);
				FRotator myRotator = myRotationVector.ToOrientationRotator();

				SetActorRotation(FQuat(myRotator));
				SetActorRotationByYaw_OnServer(myRotator.Yaw);
			}
			else
			{
				APlayerController* myController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
				if (myController)
				{
					FHitResult ResultHit;
					myController->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
					float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;

					SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
					SetActorRotationByYaw_OnServer(FindRotatorResultYaw);

					if (CurrentWeapon)
					{
						FVector Displacement = FVector(0);
						bool bIsReduceDispersion = false;
						switch (MovementState)
						{
						case EMovementState::Aim_State:
							Displacement = FVector(0.0f, 0.0f, 160.0f);
							//CurrentWeapon->ShouldReduceDispersion = true;
							bIsReduceDispersion = true;
							break;
						case EMovementState::AimWalk_State:
							Displacement = FVector(0.0f, 0.0f, 160.0f);
							//CurrentWeapon->ShouldReduceDispersion = true;
							bIsReduceDispersion = true;
							break;
						case EMovementState::Walk_State:
							Displacement = FVector(0.0f, 0.0f, 120.0f);
							//CurrentWeapon->ShouldReduceDispersion = false;
							break;
						case EMovementState::Run_State:
							Displacement = FVector(0.0f, 0.0f, 120.0f);
							//CurrentWeapon->ShouldReduceDispersion = false;
							break;
						case EMovementState::Sprint_State:
							break;
						default:
							break;
						}

						//CurrentWeapon->ShootEndLocation = ResultHit.Location + Displacement;
						CurrentWeapon->UpdateWeaponByCharacterMovementState_OnServer(ResultHit.Location + Displacement, bIsReduceDispersion);
					}
				}
			}
		}	
	}
}


EMovementState ADescent_Character::GetMovementState()
{
	return MovementState;
}

TArray<UDescent_StateEffect*> ADescent_Character::GetCurrentEffectsOnChar()
{
	return Effects;
}

int32 ADescent_Character::GetCurrentWeaponIndex()
{
	return CurrentIndexWeapon;
}

bool ADescent_Character::GetIsAlive()
{
	bool result = false;
	if (HealthComponent)
	{
		result = HealthComponent->GetIsAlive();
	}
	return result;
}

void ADescent_Character::AttackCharEvent(bool bIsFiring)
{
	AWeaponDefault* myWeapon = nullptr;
	myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->SetWeaponStateFire_OnServer(bIsFiring);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ATDSCharacter::AttackCharEvent - CurrentWeapon -NULL"));
	}
}

void ADescent_Character::CharacterUpdate()
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

void ADescent_Character::ChangeMovementState()
{
	EMovementState NewState = EMovementState::Run_State;

	if (!SprintRunEnabled && !WalkEnabled && !AimEnabled)
	{
		NewState = EMovementState::Run_State;
	}
	else
	{
		if (SprintRunEnabled)
		{
			NewState = EMovementState::Sprint_State;
			WalkEnabled = false;
			AimEnabled = false;
		}
		else
		{
			if (!SprintRunEnabled && WalkEnabled && AimEnabled)
			{
				NewState = EMovementState::AimWalk_State;
			}
			else
			{
				if (!SprintRunEnabled && WalkEnabled && !AimEnabled)
				{
					NewState = EMovementState::Walk_State;
				}
				else
				{
					if (!SprintRunEnabled && !WalkEnabled && AimEnabled)
					{
						NewState = EMovementState::Aim_State;
					}
				}
			}
		}
		
	}

	SetMovementState_OnServer(NewState);

	//CharacterUpdate();

	//weapon state
	AWeaponDefault* myWeapon = GetCurrentWeapon();
	if (myWeapon)
	{
		myWeapon->UpdateStateWeapon_OnServer(NewState);
	}
}

AWeaponDefault* ADescent_Character::GetCurrentWeapon()
{
	return CurrentWeapon;
}

void ADescent_Character::InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon)
{
	//on server
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	UDescent_GameInstance* myGI = Cast<UDescent_GameInstance>(GetGameInstance());
	FWeaponInfo myWeaponInfo;
	if (myGI)
	{
		if (myGI->GetWeaponInfoByName(IdWeaponName, myWeaponInfo))
		{
			if (myWeaponInfo.WeaponClass)
			{
				FVector SpawnLocation = FVector(0);
				FRotator SpawnRoratation = FRotator(0);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = this;
				SpawnParams.Instigator = GetInstigator();

				AWeaponDefault* myWeapon = Cast<AWeaponDefault>(GetWorld()->SpawnActor(myWeaponInfo.WeaponClass, &SpawnLocation, &SpawnRoratation, SpawnParams));
				if (myWeapon)
				{
					FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget, false);
					myWeapon->AttachToComponent(GetMesh(), Rule, FName("WeaponSocketRightHand"));
					CurrentWeapon = myWeapon;

					myWeapon->IdWeaponName = IdWeaponName;
					myWeapon->WeaponSetting = myWeaponInfo;

					myWeapon->ReloadTimeDebug = myWeaponInfo.ReloadTime;
					myWeapon->UpdateStateWeapon_OnServer(MovementState);

					myWeapon->AdditionalWeaponInfo = WeaponAdditionalInfo;
					CurrentIndexWeapon = NewCurrentIndexWeapon;

					myWeapon->OnWeaponReloadStart.AddDynamic(this, &ADescent_Character::WeaponReloadStart);
					myWeapon->OnWeaponReloadEnd.AddDynamic(this, &ADescent_Character::WeaponReloadEnd);
					myWeapon->OnWeaponFire.AddDynamic(this, &ADescent_Character::WeaponFire);


					if (CurrentWeapon->GetWeaponRound() <= 0 && CurrentWeapon->CheckCanWeaponReload())
					{
						CurrentWeapon->InitReload();
					}
					if (InventoryComponent)
					{
						InventoryComponent->OnWeaponAmmoAviable.Broadcast(myWeapon->WeaponSetting.WeaponType);
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


void ADescent_Character::TryReloadWeapon()
{
	TryReloadWeapon_OnServer();
}

void ADescent_Character::WeaponReloadStart(UAnimMontage* Anim)
{
	WeaponReloadStart_BP(Anim);
}

void ADescent_Character::WeaponReloadEnd(bool bIsSuccess, int32 AmmoTake)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->AmmoSlotChangeValue(CurrentWeapon->WeaponSetting.WeaponType, AmmoTake);
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
	}

	WeaponReloadEnd_BP(bIsSuccess);
}

void ADescent_Character::TrySwitchWeaponToIndexByKeyInput_OnServer_Implementation(int32 ToIndex)
{
	bool bIsSucces = false;
	if (CurrentWeapon && !CurrentWeapon->WeaponReloading && InventoryComponent->WeaponSlots.IsValidIndex(ToIndex))
	{
		if (CurrentIndexWeapon != ToIndex && InventoryComponent)
		{
			int32 OldIndex = CurrentIndexWeapon;
			FAdditionalWeaponInfo OldInfo;

			if (CurrentWeapon)
			{
				OldInfo = CurrentWeapon->AdditionalWeaponInfo;
				if (CurrentWeapon->WeaponReloading)
				{
					CurrentWeapon->CancelReload();
				}
			}
			
			bIsSucces = InventoryComponent->SwitchWeaponByIndex(ToIndex, OldIndex, OldInfo);
		}
	}
}

void ADescent_Character::DropCurrentWeapon()
{
	if (InventoryComponent)
	{
		InventoryComponent->DropWeaponByIndex_OnServer(CurrentIndexWeapon);
	}
}

void ADescent_Character::WeaponFire(UAnimMontage* Anim)
{
	if (InventoryComponent && CurrentWeapon)
	{
		InventoryComponent->SetAdditionalInfoWeapon(CurrentIndexWeapon, CurrentWeapon->AdditionalWeaponInfo);
	}

	WeaponFire_BP(Anim);
}

void ADescent_Character::WeaponReloadStart_BP_Implementation(UAnimMontage* Anim)
{
	//BP
}

void ADescent_Character::WeaponReloadEnd_BP_Implementation(bool bIsSuccess)
{
	//BP
}

void ADescent_Character::WeaponFire_BP_Implementation(UAnimMontage* Anim)
{
	//BP
}

UDecalComponent* ADescent_Character::GetCursorToWorld()
{
	return CurrentCursor;
}


void ADescent_Character::TryAbilityEnabled()
{
	if (AbilityEffect)
	{
		UDescent_StateEffect* newEffect = NewObject<UDescent_StateEffect>(this, AbilityEffect);
		if (newEffect)
		{
			newEffect->InitObject(this, NAME_None);
		}
	}
}

EPhysicalSurface ADescent_Character::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;
	if (HealthComponent)
	{
		if (HealthComponent->GetCurrentShield() <= 0)
		{
			if (GetMesh())
			{
				UMaterialInterface* myMaterial = GetMesh()->GetMaterial(0);
				if (myMaterial)
				{
					Result = myMaterial->GetPhysicalMaterial()->SurfaceType;
				}
			}
		}
	}

	return Result;
}

TArray<UDescent_StateEffect*> ADescent_Character::GetAllCurrentEffects()
{
	return Effects;
}

void ADescent_Character::RemoveEffect_Implementation(UDescent_StateEffect* RemoveEffect)
{
	Effects.Remove(RemoveEffect);

	SwitchEffect(RemoveEffect, false);
	EffectRemove = RemoveEffect;
}

void ADescent_Character::AddEffect_Implementation(UDescent_StateEffect* newEffect)
{
	Effects.Add(newEffect);

	SwitchEffect(newEffect, true);
	EffectAdd = newEffect;
}

void ADescent_Character::TryReloadWeapon_OnServer_Implementation()
{
	if (CurrentWeapon->GetWeaponRound() < CurrentWeapon->WeaponSetting.MaxRound && CurrentWeapon->CheckCanWeaponReload())
	{
		CurrentWeapon->InitReload();
	}
}

void ADescent_Character::SetActorRotationByYaw_OnServer_Implementation(float Yaw)
{
	SetActorRotationByYaw_Multicast(Yaw);
}

void ADescent_Character::SetActorRotationByYaw_Multicast_Implementation(float Yaw)
{
	if (Controller && !Controller->IsLocalPlayerController())
	{
		SetActorRotation(FQuat(FRotator(0.0f, Yaw, 0.0f)));
	}

}

void ADescent_Character::SetMovementState_OnServer_Implementation(EMovementState NewState)
{
	SetMovementState_Multicast(NewState);
}

void ADescent_Character::SetMovementState_Multicast_Implementation(EMovementState NewState)
{
	MovementState = NewState;
	CharacterUpdate();
}

void ADescent_Character::PlayAnim_Multicast_Implementation(UAnimMontage* Anim)
{
	if (GetMesh() && GetMesh()->GetAnimInstance())
	{
		GetMesh()->GetAnimInstance()->Montage_Play(Anim);
	}
}

void ADescent_Character::CharacterDead_BP_Implementation()
{
	//BP
}

void ADescent_Character::CharacterDead()
{
	CharacterDead_BP();

	if (HasAuthority())
	{
		float TimeAnim = 0.0f;
		int32 rnd = FMath::RandHelper(DeadsAnim.Num());
		if (DeadsAnim.IsValidIndex(rnd) && DeadsAnim[rnd] && GetMesh() && GetMesh()->GetAnimInstance())
		{
			TimeAnim = DeadsAnim[rnd]->GetPlayLength();
			//GetMesh()->GetAnimInstance()->Montage_Play(DeadsAnim[rnd]);
			PlayAnim_Multicast(DeadsAnim[rnd]);
		}

		if (GetController())
		{
			GetController()->UnPossess();
		}

		float DecraeseAnimTimer = FMath::FRandRange(0.2f, 1.0f);

		GetWorldTimerManager().SetTimer(TimerHandle_RagDollTimer, this, &ADescent_Character::EnableRagdoll_Multicast, TimeAnim - DecraeseAnimTimer, false);

		SetLifeSpan(20.0f);
		if (GetCurrentWeapon())
		{
			GetCurrentWeapon()->SetLifeSpan(20.0f);
		}
	}
	else
	{
		if (GetCursorToWorld())
		{
			GetCursorToWorld()->SetVisibility(false);
		}

		AttackCharEvent(false);
	}

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
}

void ADescent_Character::EnableRagdoll_Multicast_Implementation()
{
	if (GetMesh())
	{

		GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
		GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		GetMesh()->SetSimulatePhysics(true);
	}
}

float ADescent_Character::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
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


void ADescent_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADescent_Character, MovementState);
	DOREPLIFETIME(ADescent_Character, CurrentWeapon);
	DOREPLIFETIME(ADescent_Character, CurrentIndexWeapon);
	DOREPLIFETIME(ADescent_Character, Effects);
	DOREPLIFETIME(ADescent_Character, EffectAdd);
	DOREPLIFETIME(ADescent_Character, EffectRemove);
}

void ADescent_Character::EffectAdd_OnRep()
{
	if (EffectAdd)
	{
		SwitchEffect(EffectAdd, true);
	}
}

void ADescent_Character::EffectRemove_OnRep()
{
	if (EffectRemove)
	{
		SwitchEffect(EffectRemove, false);
	}
}

void ADescent_Character::SwitchEffect(UDescent_StateEffect* Effect, bool bIsAdd)
{
	if (bIsAdd)
	{
		if (Effect && Effect->ParticleEffect)
		{
			FName NameBoneToAttached = Effect->NameBone;
			FVector Loc = FVector(0);

			USkeletalMeshComponent* myMesh = GetMesh();
			
			if (myMesh)
			{
				UParticleSystemComponent* newParticleSystem = UGameplayStatics::UGameplayStatics::SpawnEmitterAttached(Effect->ParticleEffect, myMesh, 
					NameBoneToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
				ParticleSystemEffects.Add(newParticleSystem);
			}
		}
	}
	else
	{
		int32 i = 0;
		bool bIsFind = false;
		if (ParticleSystemEffects.Num() > 0)
		{
			while (i < ParticleSystemEffects.Num(), !bIsFind)
			{
				if (ParticleSystemEffects[i]->Template && Effect->ParticleEffect && Effect->ParticleEffect == ParticleSystemEffects[i]->Template)
				{
					bIsFind = true;
					ParticleSystemEffects[i]->DeactivateSystem();
					ParticleSystemEffects[i]->DestroyComponent();
					ParticleSystemEffects.RemoveAt(i);
				}
				i++;
			}
		}
	}
}

bool ADescent_Character::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool Wrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (int32 i = 0; i < Effects.Num(); i++)
	{
		if (Effects[i]) 
		{ 
			Wrote |= Channel->ReplicateSubobject(Effects[i], *Bunch, *RepFlags); 
		}
	}

	return Wrote;
}
