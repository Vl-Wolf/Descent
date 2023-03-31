// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileDefault.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"

// Sets default values
AProjectileDefault::AProjectileDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	BulletCollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Sphere"));
	BulletCollisionSphere->SetSphereRadius(16.0f);
	
	BulletCollisionSphere->bReturnMaterialOnMove = true;

	BulletCollisionSphere->SetCanEverAffectNavigation(false);

	RootComponent = BulletCollisionSphere;

	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bullet Projecile Mesh"));
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetCanEverAffectNavigation(false);

	BulletFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Bullet FX"));
	BulletFX->SetupAttachment(RootComponent);

	BulletProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Bullet ProjectileMovement"));
	BulletProjectileMovement->UpdatedComponent = RootComponent;
	//BulletProjectileMovement->InitialSpeed = ProjectileSetting.ProjectileInitSpeed;
	//BulletProjectileMovement->MaxSpeed = ProjectileSetting.ProjectileInitSpeed;

	BulletProjectileMovement->bRotationFollowsVelocity = true;
	BulletProjectileMovement->bShouldBounce = true;
}

// Called when the game starts or when spawned
void AProjectileDefault::BeginPlay()
{
	Super::BeginPlay();
	
	BulletCollisionSphere->OnComponentHit.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereHit);
	BulletCollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereBeginOverlap);
	BulletCollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AProjectileDefault::BulletCollisionSphereEndOverlap);

}

// Called every frame
void AProjectileDefault::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileDefault::InitProjectile(FProjectileInfo InitParam)
{
	//BulletProjectileMovement->InitialSpeed = InitParam.ProjectileInitSpeed;
	//BulletProjectileMovement->MaxSpeed = InitParam.ProjectileMaxSpeed;
	this->SetLifeSpan(InitParam.ProjectileLifeTime);
	

	if (InitParam.ProjectileStaticMesh)
	{
		InitVisualMeshProjectile_Multicast(InitParam.ProjectileStaticMesh, InitParam.ProjectileStaticMeshOffset);
	}
	else
	{
		BulletMesh->DestroyComponent();
	}

	if (InitParam.ProjectileTrailFX)
	{
		InitVisualTrailProjectile_Multicast(InitParam.ProjectileTrailFX, InitParam.ProjectileTrailFXOffset);
	}
	else
	{
		BulletFX->DestroyComponent();
	}


	InitVelocity_Multicast(InitParam.ProjectileInitSpeed, InitParam.ProjectileMaxSpeed);

	ProjectileSetting = InitParam;
}

void AProjectileDefault::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && Hit.PhysMaterial.IsValid())
	{
		EPhysicalSurface mySurfaceType = UGameplayStatics::GetSurfaceType(Hit);

		if (ProjectileSetting.HitDecals.Contains(mySurfaceType))
		{
			UMaterialInterface* myMaterial = ProjectileSetting.HitDecals[mySurfaceType];

			if (myMaterial && OtherComp)
			{
				SpawnHitDecal_Multicast(myMaterial, OtherComp, Hit);
			}
		}
		if (ProjectileSetting.HitFXs.Contains(mySurfaceType))
		{
			UParticleSystem* myParticle = ProjectileSetting.HitFXs[mySurfaceType];
			{
				if (myParticle)
				{
					SpawnHitFX_Multicast(myParticle, Hit);
				}
			}
		}

		if (ProjectileSetting.HitSound)
		{
			SpawnHitSound_Multicast(ProjectileSetting.HitSound, Hit);
		}

		UTypes::AddEffectBySurfaceType(Hit.GetActor(), Hit.BoneName, ProjectileSetting.Effect, mySurfaceType);
	}
	UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileSetting.ProjectileDamage, Hit.TraceStart, Hit, GetInstigatorController(), this, NULL);
	UAISense_Damage::ReportDamageEvent(GetWorld(), Hit.GetActor(), GetInstigator(), ProjectileSetting.ProjectileDamage, Hit.Location, Hit.Location);
	ImpactProjectile();
}

void AProjectileDefault::BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFroomSweep, const FHitResult& SweepResult)
{
}

void AProjectileDefault::BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AProjectileDefault::ImpactProjectile()
{
	this->Destroy();
}


void AProjectileDefault::SpawnHitDecal_Multicast_Implementation(UMaterialInterface* DecalMaterial, UPrimitiveComponent* OtherComponet, FHitResult HitResult)
{
	UGameplayStatics::SpawnDecalAttached(DecalMaterial, FVector(20.0f), OtherComponet, NAME_None, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
}

void AProjectileDefault::SpawnHitFX_Multicast_Implementation(UParticleSystem* FXTemplate, FHitResult HitResult)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FXTemplate, FTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint, FVector(1.0f)));
}

void AProjectileDefault::SpawnHitSound_Multicast_Implementation(USoundBase* HitSound, FHitResult HitResult)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitResult.ImpactPoint);

}

void AProjectileDefault::InitVisualMeshProjectile_Multicast_Implementation(UStaticMesh* newMesh, FTransform MeshRelative)
{
	BulletMesh->SetStaticMesh(newMesh);
	BulletMesh->SetRelativeTransform(MeshRelative);
}

void AProjectileDefault::InitVisualTrailProjectile_Multicast_Implementation(UParticleSystem* newTrail, FTransform TrailRelative)
{
	BulletFX->SetTemplate(newTrail);
	BulletFX->SetRelativeTransform(TrailRelative);
	
}

void AProjectileDefault::InitVelocity_Multicast_Implementation(float InitSpeed, float MaxSpeed)
{
	if (BulletProjectileMovement)
	{
		BulletProjectileMovement->Velocity = GetActorForwardVector() * InitSpeed;
		BulletProjectileMovement->MaxSpeed = MaxSpeed;
		BulletProjectileMovement->InitialSpeed = InitSpeed;
	}
}

void AProjectileDefault::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	if (BulletProjectileMovement)
	{
		BulletProjectileMovement->Velocity = NewVelocity;
		
	}
}
