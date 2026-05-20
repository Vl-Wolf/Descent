// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDefault.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "TDS/Character/TDSInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Projectiles/ProjectileDefault.h"


AWeaponDefault::AWeaponDefault()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	SkeletalMeshWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMeshWeapon->SetGenerateOverlapEvents(false);
	SkeletalMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	SkeletalMeshWeapon->SetupAttachment(RootComponent);

	StaticMeshWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshWeapon->SetGenerateOverlapEvents(false);
	StaticMeshWeapon->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshWeapon->SetupAttachment(RootComponent);

	ShootLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ShootLocation"));
	ShootLocation->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AWeaponDefault::BeginPlay()
{
	Super::BeginPlay();
	
	WeaponInit();
}


void AWeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
		DispersionTick(DeltaTime);
}

void AWeaponDefault::DispersionTick(float DeltaTime)
{
	if (!WeaponReloading)
	{
		if (!WeaponFiring)
		{
			if (ShouldReduceDispersion)
			{
				CurrentDispersion -= CurrentDispersionReduction;
			}
			else
			{
				CurrentDispersion += CurrentDispersionReduction;
			}
		}
		if (CurrentDispersion < CurrentDispersionMin)
		{
			CurrentDispersion = CurrentDispersionMin;
		}
		else
		{
			if (CurrentDispersion > CurrentDispersionMax)
			{
				CurrentDispersion = CurrentDispersionMax;
			}
		}
	}
	if (ShowDebug)
		UE_LOG(LogTemp, Warning, TEXT("Dispersion: MAX = %f. MIN = %f. Current = %f"), CurrentDispersionMax, CurrentDispersionMin, CurrentDispersion);
}

void AWeaponDefault::WeaponInit()
{
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}
	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent();
	}

	UpdateStateWeapon_OnServer(EMovementState::Run_State);
}

void AWeaponDefault::SetWeaponStateFire_OnServer_Implementation(bool bIsFire)
{
	if (CheckWeaponCanFire() && bIsFire && GetWeaponRound() > 0 && !WeaponReloading)
	{
		WeaponFiring = true;
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, 
			&AWeaponDefault::Fire, WeaponSetting.RateOfFire, true, 0.0f);
	}
	else
	{
		WeaponFiring = false;
		GetWorldTimerManager().ClearTimer(FireTimerHandle);
	}
}

bool AWeaponDefault::CheckWeaponCanFire()
{
	return !BlockFire;
}

FProjectileInfo AWeaponDefault::GetProjectile()
{
	return WeaponSetting.ProjectileSetting;
}

void AWeaponDefault::Fire()
{
	//on server by weapon bool
	
	UAnimMontage* AnimPlay = nullptr;
	if (WeaponAiming)
	{
		AnimPlay = WeaponSetting.AnimationWeaponInfo.AnimCharacterFireAim;
	}
	else
	{
		AnimPlay = WeaponSetting.AnimationWeaponInfo.AnimCharacterFire;
	}

	if (WeaponSetting.AnimationWeaponInfo.AnimWeaponFire)
	{
		AnimWeaponStart_Multicast(WeaponSetting.AnimationWeaponInfo.AnimWeaponFire);
	}
	

	if (WeaponSetting.ShellBullets.DropMesh)
	{
		if (WeaponSetting.ShellBullets.DropTime > 0.0f)
		{
			GetWorldTimerManager().SetTimer(ShellDropTimerHandle, this, &AWeaponDefault::DropShell, 
				WeaponSetting.ShellBullets.DropTime, false);
		}
		else
			DropShell();
	}
	
	AdditionalWeaponInfo.Round = AdditionalWeaponInfo.Round - 1;
	ChangeDispersionByShoot();
	
	OnWeaponFire.Broadcast(AnimPlay);

	FXWeaponFire_Multicast(WeaponSetting.EffectFireWeapon, WeaponSetting.SoundFireWeapon);

	int8 NumberProjectile = GetNumberProjectileByShoot();

	if (ShootLocation)
	{
		FVector SpawnLocation = ShootLocation->GetComponentLocation();
		FRotator SpawnRotation = ShootLocation->GetComponentRotation();
		FProjectileInfo ProjectileInfo;
		ProjectileInfo = GetProjectile();
		FVector EndLocation;

		for (int8 i = 0; i < NumberProjectile; i++)
		{
			EndLocation = GetFireEndLocation();
			
			if (ProjectileInfo.Projectile)
			{
				FVector Dir = EndLocation - SpawnLocation;
				Dir.Normalize();
				FMatrix Matrix(Dir, FVector(0, 1, 0), FVector(0, 0, 1), FVector::ZeroVector);
				SpawnRotation = Matrix.Rotator();

				//basic fire
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();

				AProjectileDefault* Projectile = Cast<AProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (Projectile)
				{
					Projectile->InitProjectile(WeaponSetting.ProjectileSetting);
				}
			}
			else
			{
				FHitResult Hit;
				TArray<AActor*> Actors;
				
				EDrawDebugTrace::Type DebugTrace;
				if (ShowDebug)
				{
					DrawDebugLine(GetWorld(), SpawnLocation, SpawnLocation + ShootLocation->GetForwardVector() * WeaponSetting.DistanceTrace,
						FColor::Black, false, 5.0f, (uint8)'\000', 0.5f);
					DebugTrace = EDrawDebugTrace::ForDuration;
				}
				else
				{
					DebugTrace = EDrawDebugTrace::None;
				}

				UKismetSystemLibrary::LineTraceSingle(GetWorld(), SpawnLocation, EndLocation * WeaponSetting.DistanceTrace, 
													ETraceTypeQuery::TraceTypeQuery4, false, Actors, EDrawDebugTrace::ForDuration, 
													Hit, true, FLinearColor::Red, FLinearColor::Green, 5.0f);

				

				if (Hit.GetActor() && Hit.PhysMaterial.IsValid())
				{
					EPhysicalSurface SurfaceType = UGameplayStatics::GetSurfaceType(Hit);

					if (WeaponSetting.ProjectileSetting.HitDecals.Contains(SurfaceType))
					{
						UMaterialInterface* Material = WeaponSetting.ProjectileSetting.HitDecals[SurfaceType];

						if (Material && Hit.GetComponent())
						{
							SpawnTraceHitDecal_Multicast(Material, Hit);
						}
					}
					if (WeaponSetting.ProjectileSetting.HitFXs.Contains(SurfaceType))
					{
						UParticleSystem* Particle = WeaponSetting.ProjectileSetting.HitFXs[SurfaceType];
						if (Particle)
						{
							SpawnTraceHitFX_Multicast(Particle, Hit);
						}
						
					}

					if (WeaponSetting.ProjectileSetting.HitSound)
					{
						SpawnTraceHitSound_Multicast(WeaponSetting.ProjectileSetting.HitSound, Hit);
					}

					UTypes::AddEffectBySurfaceType(Hit.GetActor(), Hit.BoneName, ProjectileInfo.Effect, SurfaceType);

					UGameplayStatics::ApplyPointDamage(Hit.GetActor(), WeaponSetting.ProjectileSetting.ProjectileDamage, Hit.TraceStart, Hit, GetInstigatorController(), this, NULL);
				}
			}

		}
		
	}

	if (GetWeaponRound() <= 0 && !WeaponReloading)
	{
		GetWorldTimerManager().ClearTimer(FireTimerHandle);
		WeaponFiring = false;
		
		if (CheckCanWeaponReload())
			InitReload();
		
		return;
	}
}

void AWeaponDefault::UpdateStateWeapon_OnServer_Implementation(EMovementState NewMovementState)
{
	BlockFire = false;
	
	switch (NewMovementState)
	{
	case EMovementState::Aim_State:
		WeaponAiming = true;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::AimWalk_State:
		WeaponAiming = true;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.AimWalk_StateDispersionReduction;
		break;
	case EMovementState::Walk_State:
		WeaponAiming = false;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Walk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Walk_StateDispersionReduction;
		break;
	case EMovementState::Run_State:
		WeaponAiming = false;
		CurrentDispersionMax = WeaponSetting.DispersionWeapon.Run_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispersionWeapon.Run_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispersionWeapon.Run_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispersionWeapon.Run_StateDispersionReduction;
		break;
	case EMovementState::Sprint_State:
		WeaponAiming = false;
		BlockFire = true;
		SetWeaponStateFire_OnServer(false);
		break;
	default:
		break;
	}
}

void AWeaponDefault::ChangeDispersionByShoot()
{
	CurrentDispersion += CurrentDispersionRecoil;
}

float AWeaponDefault::GetCurrentDispersion() const
{
	float Result = CurrentDispersion;
	return Result;
}

FVector AWeaponDefault::ApplyDispersionToShoot(FVector DirectionShoot) const
{
	return FMath::VRandCone(DirectionShoot, GetCurrentDispersion() * PI/180.0f);
}

FVector AWeaponDefault::GetFireEndLocation() const
{
	bool bShootDirection = false;
	FVector EndLocation = FVector(0.0f);

	FVector tmpV = (ShootLocation->GetComponentLocation() - ShootEndLocation);
	if (tmpV.Size() > SizeVectorToChangeShootDirectionLogic)
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot((ShootLocation->GetComponentLocation() - ShootEndLocation).GetSafeNormal()) * -20000.0f;
		if (ShowDebug)
		{
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), -(ShootLocation->GetComponentLocation() - ShootEndLocation), WeaponSetting.DistanceTrace, GetCurrentDispersion() * PI / 180.0f, GetCurrentDispersion() * PI / 180.0f, 32, FColor::Emerald, false, 0.1f, (uint8)'\000', 1.0f);
		}
	}
	else
	{
		EndLocation = ShootLocation->GetComponentLocation() + ApplyDispersionToShoot(ShootLocation->GetForwardVector()) * 20000.0f;
		if (ShowDebug)
		{
			DrawDebugCone(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetForwardVector(), WeaponSetting.DistanceTrace, GetCurrentDispersion() * PI / 180.f, GetCurrentDispersion() * PI / 180.f, 32, FColor::Emerald, false, .1f, (uint8)'\000', 1.0f);

		}

	}

	if (ShowDebug)
	{
		//direction weapon look
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootLocation->GetComponentLocation() + ShootLocation->GetForwardVector() * 500.0f, FColor::Cyan, false, 5.0f, (uint8)'\000', 0.5f);
		//direction projectile must fly
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), ShootEndLocation, FColor::Red, false, 5.0f, (uint8)'\000', 0.5f);
		//direction projectile current fly
		DrawDebugLine(GetWorld(), ShootLocation->GetComponentLocation(), EndLocation, FColor::Black, false, 5.0f, (uint8)'\000', 0.5f);

	}

	return EndLocation;
}

int8 AWeaponDefault::GetNumberProjectileByShoot() const
{
	return WeaponSetting.NumberProjectileByShot;
}

int32 AWeaponDefault::GetWeaponRound()
{
	return AdditionalWeaponInfo.Round;
}

void AWeaponDefault::InitReload()
{
	//on server
	WeaponReloading = true;
		
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
	WeaponFiring = false;
	
	GetWorldTimerManager().SetTimer(ReloadTimerHandle, this, &AWeaponDefault::FinishReload, 
		ReloadTime, false);

	UAnimMontage* AnimPlay = WeaponAiming ? WeaponSetting.AnimationWeaponInfo.AnimCharacterReloadAim : WeaponSetting.AnimationWeaponInfo.AnimCharacterReload;

	OnWeaponReloadStart.Broadcast(AnimPlay);

	UAnimMontage* AnimWeaponPlay = WeaponAiming ? WeaponSetting.AnimationWeaponInfo.AnimWeaponReloadAim : WeaponSetting.AnimationWeaponInfo.AnimWeaponReload;
	
	if (WeaponSetting.AnimationWeaponInfo.AnimWeaponReload && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		AnimWeaponStart_Multicast(AnimWeaponPlay);
	}

	if (WeaponSetting.MagazineDrop.DropMesh)
	{
		GetWorldTimerManager().SetTimer(DropMagazineTimerHandle, this, &AWeaponDefault::DropMagazine, 
			WeaponSetting.MagazineDrop.DropTime, false);
	}
}

void AWeaponDefault::FinishReload()
{
	WeaponReloading = false;
	
	int8 AvailableAmmoFromInventory = GetAvailableAmmoForReload();
	int8 AmmoNeedTakeFromInventory;
	int8 NeedToReload = WeaponSetting.MaxRound - AdditionalWeaponInfo.Round;

	if (NeedToReload > AvailableAmmoFromInventory)
	{
		AdditionalWeaponInfo.Round += AvailableAmmoFromInventory;
		AmmoNeedTakeFromInventory = AvailableAmmoFromInventory;
	}
	else
	{
		AdditionalWeaponInfo.Round += NeedToReload;
		AmmoNeedTakeFromInventory = NeedToReload;
	}

	OnWeaponReloadEnd.Broadcast(true, -AmmoNeedTakeFromInventory);
}

void AWeaponDefault::CancelReload()
{
	WeaponReloading = false;
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);
	GetWorldTimerManager().ClearTimer(DropMagazineTimerHandle);
	
	if (SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->StopAllMontages(0.15f);
	}

	OnWeaponReloadEnd.Broadcast(false, 0);
}

bool AWeaponDefault::CheckCanWeaponReload()
{
	bool Result = true; 
	if (GetOwner())
	{
		UTDSInventoryComponent* Inventory = Cast<UTDSInventoryComponent>(GetOwner()->GetComponentByClass(UTDSInventoryComponent::StaticClass()));
		if (Inventory)
		{
			int8 AvailableAmmoForWeapon;
			if (!Inventory->CheckAmmoForWeapon(WeaponSetting.WeaponType, AvailableAmmoForWeapon))
			{
				Result = false;
				Inventory->OnWeaponNotHaveRound.Broadcast(Inventory->GetWeaponIndexSlotByName(IdWeaponName));

			}
			else
			{
				Inventory->OnWeaponHaveRound.Broadcast(Inventory->GetWeaponIndexSlotByName(IdWeaponName));
			}
		}
	}
	return Result;
}

int8 AWeaponDefault::GetAvailableAmmoForReload()
{
	int8 AvailableAmmoForWeapon = WeaponSetting.MaxRound;
	if (GetOwner())
	{
		UTDSInventoryComponent* Inventory = Cast<UTDSInventoryComponent>(GetOwner()->GetComponentByClass(UTDSInventoryComponent::StaticClass()));
		if (Inventory)
		{
			if (Inventory->CheckAmmoForWeapon(WeaponSetting.WeaponType, AvailableAmmoForWeapon))
			{
				AvailableAmmoForWeapon = AvailableAmmoForWeapon;
			}
		}
	}
	return AvailableAmmoForWeapon;
}

void AWeaponDefault::DropMagazine()
{
	InitDropMesh_OnServer(
		WeaponSetting.MagazineDrop.DropMesh, 
		WeaponSetting.MagazineDrop.DropMeshOffset, 
		WeaponSetting.MagazineDrop.DropMeshImpulseDirection, 
		WeaponSetting.MagazineDrop.DropTime, 
		WeaponSetting.MagazineDrop.DropMeshLifeTime, 
		WeaponSetting.MagazineDrop.MassMesh, 
		WeaponSetting.MagazineDrop.PowerImpulse, 
		WeaponSetting.MagazineDrop.ImpulseRandomDispersion);
}

void AWeaponDefault::DropShell()
{
	InitDropMesh_OnServer(
		WeaponSetting.ShellBullets.DropMesh, 
		WeaponSetting.ShellBullets.DropMeshOffset, 
		WeaponSetting.ShellBullets.DropMeshImpulseDirection, 
		WeaponSetting.ShellBullets.DropTime, 
		WeaponSetting.ShellBullets.DropMeshLifeTime, 
		WeaponSetting.ShellBullets.MassMesh, 
		WeaponSetting.ShellBullets.PowerImpulse, 
		WeaponSetting.ShellBullets.ImpulseRandomDispersion);
}

void AWeaponDefault::FXWeaponFire_Multicast_Implementation(UParticleSystem* FXFire, USoundBase* SoundFire)
{
	if (SoundFire)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), SoundFire, ShootLocation->GetComponentLocation());
	}
	if (FXFire)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FXFire, ShootLocation->GetComponentTransform());
	}
}

void AWeaponDefault::InitDropMesh_OnServer_Implementation(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float DropTime, float LifeTimeMesh, float MassMesh, float PowerImpulse, float ImpulseRandomDispersion)
{
	if (DropMesh)
	{
		FTransform Transform;
		FVector LocalDir = this->GetActorForwardVector() * Offset.GetLocation().X + this->GetActorRightVector() * Offset.GetLocation().Y + this->GetActorUpVector() * Offset.GetLocation().Z;

		Transform.SetLocation(GetActorLocation() + LocalDir);
		Transform.SetScale3D(Offset.GetScale3D());

		Transform.SetRotation((GetActorRotation() + Offset.Rotator()).Quaternion());
		

		ShellDropFire_Multicast(DropMesh, Transform, DropImpulseDirection, DropTime, LifeTimeMesh, MassMesh, PowerImpulse, ImpulseRandomDispersion, LocalDir);
	}
}

void AWeaponDefault::ShellDropFire_Multicast_Implementation(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float DropTime, float LifeTimeMesh, float MassMesh, float PowerImpulse, float ImpulseRandomDispersion, FVector LocalDir)
{
	AStaticMeshActor* NewActor = nullptr;

	FActorSpawnParameters Parameters;
	Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	Parameters.Owner = this;

	NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Offset, Parameters);

	if (NewActor && NewActor->GetStaticMeshComponent())
	{
		NewActor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("IgnoredOnlyPawn"));
		NewActor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		NewActor->SetActorTickEnabled(false);
		NewActor->InitialLifeSpan = LifeTimeMesh;
		NewActor->GetStaticMeshComponent()->Mobility = EComponentMobility::Movable;
		NewActor->GetStaticMeshComponent()->SetSimulatePhysics(true);
		NewActor->GetStaticMeshComponent()->SetStaticMesh(DropMesh);

		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Ignore);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		NewActor->GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECollisionResponse::ECR_Block);

		if (MassMesh > 0.0f)
		{
			NewActor->GetStaticMeshComponent()->SetMassOverrideInKg(NAME_None, MassMesh, true);
		}

		if (!DropImpulseDirection.IsNearlyZero())
		{
			FVector FinalDir;
			LocalDir = LocalDir + (DropImpulseDirection * 1000.0f);

			if (!FMath::IsNearlyZero(ImpulseRandomDispersion))
			{
				FinalDir += UKismetMathLibrary::RandomUnitVectorInConeInDegrees(LocalDir, ImpulseRandomDispersion);
			}
			FinalDir.GetSafeNormal(0.0001f);
			NewActor->GetStaticMeshComponent()->AddImpulse(FinalDir * PowerImpulse);
		}
	}
}

void AWeaponDefault::AnimWeaponStart_Multicast_Implementation(UAnimMontage* Anim)
{
	if (Anim && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(Anim);
	}
}

void AWeaponDefault::UpdateWeaponByCharacterMovementState_OnServer_Implementation(FVector NewShootEndLocation, bool NewShouldReduceDispersion)
{
	ShootEndLocation = NewShootEndLocation;
	ShouldReduceDispersion = NewShouldReduceDispersion;

}

void AWeaponDefault::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponDefault, AdditionalWeaponInfo);
	DOREPLIFETIME(AWeaponDefault, WeaponReloading);
	DOREPLIFETIME(AWeaponDefault, ShootEndLocation);

}

void AWeaponDefault::SpawnTraceHitDecal_Multicast_Implementation(UMaterialInterface* DecalMaterial, FHitResult HitResult)
{
	UGameplayStatics::SpawnDecalAttached(DecalMaterial, FVector(20.0f), HitResult.GetComponent(), NAME_None, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
}

void AWeaponDefault::SpawnTraceHitFX_Multicast_Implementation(UParticleSystem* FXTemplate, FHitResult HitResult)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FXTemplate, FTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint, FVector(1.0f)));
}

void AWeaponDefault::SpawnTraceHitSound_Multicast_Implementation(USoundBase* HitSound, FHitResult HitResult)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitResult.ImpactPoint);
}
