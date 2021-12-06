// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponDefault.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"
#include "TDS/Character/TDSInventoryComponent.h"

// Sets default values
AWeaponDefault::AWeaponDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

// Called every frame
void AWeaponDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FireTick(DeltaTime);
	ReloadTick(DeltaTime);
	DispersionTick(DeltaTime);
	DropTick(DeltaTime);
	ShellDropTick(DeltaTime);
}

void AWeaponDefault::FireTick(float DeltaTime)
{
	if (WeaponFiring && GetWeaponRound() > 0 && !WeaponReloading)
	{
		if (FireTimer < 0.0f)
		{
			Fire();
		}
		else
		{
			FireTimer -= DeltaTime;
		}
	}
	
}

void AWeaponDefault::ReloadTick(float DeltaTime)
{
	if (WeaponReloading)
	{
		if (ReloadTimer < 0.0f)
		{
			FinishReload();
		}
		else
		{
			ReloadTimer -= DeltaTime;
		}
	}
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

void AWeaponDefault::DropTick(float DeltaTime)
{
	if (DropMagasinFlag)
	{
		if (DropMagasinTimer < 0.0f)
		{
			DropMagasinFlag = false;
			InitDropMesh(WeaponSetting.MagasinDrop.DropMesh, WeaponSetting.MagasinDrop.DropMeshOffset, WeaponSetting.MagasinDrop.DropMeshImpulseDirection, WeaponSetting.MagasinDrop.DropTime, WeaponSetting.MagasinDrop.DropMeshLifeTime, WeaponSetting.MagasinDrop.MassMesh, WeaponSetting.MagasinDrop.PowerImpulse, WeaponSetting.MagasinDrop.ImpulseRandomDispersion);
		}
		else
		{
			DropMagasinTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::ShellDropTick(float DeltaTime)
{
	if (DropShellBulletsFlag)
	{
		if (DropShellBulletsTimer < 0.0f)
		{
			DropShellBulletsFlag = false;
			InitDropMesh(WeaponSetting.ShellBullets.DropMesh, WeaponSetting.ShellBullets.DropMeshOffset, WeaponSetting.ShellBullets.DropMeshImpulseDirection, WeaponSetting.ShellBullets.DropTime, WeaponSetting.ShellBullets.DropMeshLifeTime, WeaponSetting.ShellBullets.MassMesh, WeaponSetting.ShellBullets.PowerImpulse, WeaponSetting.ShellBullets.ImpulseRandomDispersion);
		}
		else
		{
			DropShellBulletsTimer -= DeltaTime;
		}
	}
}

void AWeaponDefault::WeaponInit()
{
	if (SkeletalMeshWeapon && !SkeletalMeshWeapon->SkeletalMesh)
	{
		SkeletalMeshWeapon->DestroyComponent(true);
	}
	if (StaticMeshWeapon && !StaticMeshWeapon->GetStaticMesh())
	{
		StaticMeshWeapon->DestroyComponent(true);
	}

	UpdateStateWeapon(EMovementState::Run_State);
}

void AWeaponDefault::SetWeaponStateFire(bool bIsFire)
{
	if (CheckWeaponCanFire())
	{
		WeaponFiring = bIsFire;
	}
	else
	{
		WeaponFiring = false;
		FireTimer = 0.01f;//Remove
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
	UAnimMontage* AnimPlay = nullptr;
	if (WeaponAiming)
	{
		AnimPlay = WeaponSetting.AnimationWeaponInfo.AnimCharacterFireAim;
	}
	else
	{
		AnimPlay = WeaponSetting.AnimationWeaponInfo.AnimCharacterFire;
	}

	if (WeaponSetting.AnimationWeaponInfo.AnimWeaponFire && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(WeaponSetting.AnimationWeaponInfo.AnimWeaponFire);
	}

	if (WeaponSetting.ShellBullets.DropMesh)
	{
		if (WeaponSetting.ShellBullets.DropTime < 0.0f)
		{
			InitDropMesh(WeaponSetting.ShellBullets.DropMesh, WeaponSetting.ShellBullets.DropMeshOffset, WeaponSetting.ShellBullets.DropMeshImpulseDirection, WeaponSetting.ShellBullets.DropTime, WeaponSetting.ShellBullets.DropMeshLifeTime, WeaponSetting.ShellBullets.MassMesh, WeaponSetting.ShellBullets.PowerImpulse, WeaponSetting.ShellBullets.ImpulseRandomDispersion);
		}
		else
		{
			DropShellBulletsFlag = true;
			DropShellBulletsTimer = WeaponSetting.ShellBullets.DropTime;
		}
	}

	OnWeaponFire.Broadcast(AnimPlay);

	FireTimer = WeaponSetting.RateOfFire;
	AdditionalWeaponInfo.Round = AdditionalWeaponInfo.Round - 1;
	ChangeDispersionByShoot();

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), WeaponSetting.SoundFireWeapon, ShootLocation->GetComponentLocation());
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponSetting.EffectFireWeapon, ShootLocation->GetComponentTransform());
	
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
				FMatrix myMatrix(Dir, FVector(0, 1, 0), FVector(0, 0, 1), FVector::ZeroVector);
				SpawnRotation = myMatrix.Rotator();

				//basic fire
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = GetInstigator();

				AProjectileDefault* myProjectile = Cast<AProjectileDefault>(GetWorld()->SpawnActor(ProjectileInfo.Projectile, &SpawnLocation, &SpawnRotation, SpawnParams));
				if (myProjectile)
				{
					myProjectile->InitProjectile(WeaponSetting.ProjectileSetting);
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
					EPhysicalSurface mySurfaceType = UGameplayStatics::GetSurfaceType(Hit);

					if (WeaponSetting.ProjectileSetting.HitDecals.Contains(mySurfaceType))
					{
						UMaterialInterface* myMaterial = WeaponSetting.ProjectileSetting.HitDecals[mySurfaceType];

						if (myMaterial && Hit.GetComponent())
						{
							UGameplayStatics::SpawnDecalAttached(myMaterial, FVector(20.0f), Hit.GetComponent(), NAME_None, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
						}
					}
					if (WeaponSetting.ProjectileSetting.HitFXs.Contains(mySurfaceType))
					{
						UParticleSystem* myParticle = WeaponSetting.ProjectileSetting.HitFXs[mySurfaceType];
						if (myParticle)
						{
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myParticle, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(1.0f)));
						}
						
					}

					if (WeaponSetting.ProjectileSetting.HitSound)
					{
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponSetting.ProjectileSetting.HitSound, Hit.ImpactPoint);
					}

					UTypes::AddEffectBySurfaceType(Hit.GetActor(), Hit.BoneName, ProjectileInfo.Effect, mySurfaceType);

					UGameplayStatics::ApplyPointDamage(Hit.GetActor(), WeaponSetting.ProjectileSetting.ProjectileDamage, Hit.TraceStart, Hit, GetInstigatorController(), this, NULL);
				}
			}

		}
		
	}

	if (GetWeaponRound() <= 0 && !WeaponReloading)
	{
		if (CheckCanWeaponReload())
		{
			InitReload();
		}
	}
}

void AWeaponDefault::UpdateStateWeapon(EMovementState NewMovementState)
{
	BlockFire = false;
	
	switch (NewMovementState)
	{
	case EMovementState::Aim_State:
		WeaponAiming = true;
		CurrentDispersionMax = WeaponSetting.DispesionWeapon.Aim_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispesionWeapon.Aim_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispesionWeapon.Aim_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispesionWeapon.Aim_StateDispersionReduction;
		break;
	case EMovementState::AimWalk_State:
		WeaponAiming = true;
		CurrentDispersionMax = WeaponSetting.DispesionWeapon.AimWalk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispesionWeapon.AimWalk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispesionWeapon.AimWalk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispesionWeapon.AimWalk_StateDispersionReduction;
		break;
	case EMovementState::Walk_State:
		WeaponAiming = false;
		CurrentDispersionMax = WeaponSetting.DispesionWeapon.Walk_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispesionWeapon.Walk_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispesionWeapon.Walk_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispesionWeapon.Walk_StateDispersionReduction;
		break;
	case EMovementState::Run_State:
		WeaponAiming = false;
		CurrentDispersionMax = WeaponSetting.DispesionWeapon.Run_StateDispersionAimMax;
		CurrentDispersionMin = WeaponSetting.DispesionWeapon.Run_StateDispersionAimMin;
		CurrentDispersionRecoil = WeaponSetting.DispesionWeapon.Run_StateDispersionAimRecoil;
		CurrentDispersionReduction = WeaponSetting.DispesionWeapon.Run_StateDispersionReduction;
		break;
	case EMovementState::Sprint_State:
		WeaponAiming = false;
		BlockFire = true;
		SetWeaponStateFire(false);
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
	//UE_LOG(LogTemp, Warning, TEXT("Vector: X = %f. Y = %f. Size = %f"), tmpV.X, tmpV.Y, tmpV.Size());
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
	WeaponReloading = true;

	ReloadTimer = WeaponSetting.ReloadTime;

	UAnimMontage* AnimPlay = nullptr;

	if (WeaponAiming)
	{
		AnimPlay = WeaponSetting.AnimationWeaponInfo.AnimCharacterReloadAim;
	}
	else
	{
		AnimPlay = WeaponSetting.AnimationWeaponInfo.AnimCharacterReload;
	}

	OnWeaponReloadStart.Broadcast(AnimPlay);

	UAnimMontage* AnimWeaponPlay = nullptr;

	if (WeaponAiming)
	{
		AnimWeaponPlay = WeaponSetting.AnimationWeaponInfo.AnimWeaponReloadAim;
	}
	else
	{
		AnimWeaponPlay = WeaponSetting.AnimationWeaponInfo.AnimWeaponReload;
	}

	if (WeaponSetting.AnimationWeaponInfo.AnimWeaponReload && SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->Montage_Play(AnimWeaponPlay);
	}

	if (WeaponSetting.MagasinDrop.DropMesh)
	{
		DropMagasinFlag = true;
		DropMagasinTimer = WeaponSetting.MagasinDrop.DropTime;
	}
}

void AWeaponDefault::FinishReload()
{
	WeaponReloading = false;
	
	int8 AviableAmmoFromInventory = GetAviableAmmoForReload();
	int8 AmmoNeedTakeFromInventory;
	int8 NeedToReload = WeaponSetting.MaxRound - AdditionalWeaponInfo.Round;

	if (NeedToReload > AviableAmmoFromInventory)
	{
		AdditionalWeaponInfo.Round = AviableAmmoFromInventory;
		AmmoNeedTakeFromInventory = AviableAmmoFromInventory;
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
	if (SkeletalMeshWeapon && SkeletalMeshWeapon->GetAnimInstance())
	{
		SkeletalMeshWeapon->GetAnimInstance()->StopAllMontages(0.15f);
	}

	OnWeaponReloadEnd.Broadcast(false, 0);
	DropMagasinFlag = false;
}

bool AWeaponDefault::CheckCanWeaponReload()
{
	bool result = true; 
	if (GetOwner())
	{
		UTDSInventoryComponent* myInv = Cast<UTDSInventoryComponent>(GetOwner()->GetComponentByClass(UTDSInventoryComponent::StaticClass()));
		if (myInv)
		{
			int8 AviableAmmoForWeapon;
			if (!myInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
			{
				result = false;
				myInv->OnWeaponNotHaveRound.Broadcast(myInv->GetWeaponIndexSlotByName(IdWeaponName));

			}
			else
			{
				myInv->OnWeaponHaveRound.Broadcast(myInv->GetWeaponIndexSlotByName(IdWeaponName));
			}
		}
	}
	return result;
}

int8 AWeaponDefault::GetAviableAmmoForReload()
{
	int8 AviableAmmoForWeapon = WeaponSetting.MaxRound;
	if (GetOwner())
	{
		UTDSInventoryComponent* myInv = Cast<UTDSInventoryComponent>(GetOwner()->GetComponentByClass(UTDSInventoryComponent::StaticClass()));
		if (myInv)
		{
			if (myInv->CheckAmmoForWeapon(WeaponSetting.WeaponType, AviableAmmoForWeapon))
			{
				AviableAmmoForWeapon = AviableAmmoForWeapon;
			}
		}
	}
	return AviableAmmoForWeapon;
}

void AWeaponDefault::InitDropMesh(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float DropTime, float LifeTimeMesh, float MassMesh, float PowerImpulse, float ImpulseRandomDispersion)
{
	if (DropMesh)
	{
		FTransform Transform;
		FVector LocalDir = this->GetActorForwardVector() * Offset.GetLocation().X + this->GetActorRightVector() * Offset.GetLocation().Y + this->GetActorUpVector() * Offset.GetLocation().Z;

		Transform.SetLocation(GetActorLocation() + LocalDir);
		Transform.SetScale3D(Offset.GetScale3D());

		Transform.SetRotation((GetActorRotation() + Offset.Rotator()).Quaternion());
		AStaticMeshActor* NewActor = nullptr;

		FActorSpawnParameters Parameters;
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		Parameters.Owner = this;
		NewActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), Transform, Parameters);

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
				NewActor->GetStaticMeshComponent()->AddImpulse(FinalDir* PowerImpulse);
			}
		}
	}
}

