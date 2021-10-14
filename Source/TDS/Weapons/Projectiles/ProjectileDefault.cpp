// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileDefault.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectileDefault::AProjectileDefault()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	BulletProjectileMovement->InitialSpeed = ProjectileSetting.ProjectileInitSpeed;
	BulletProjectileMovement->MaxSpeed = ProjectileSetting.ProjectileInitSpeed;

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
	BulletProjectileMovement->InitialSpeed = InitParam.ProjectileInitSpeed;
	BulletProjectileMovement->MaxSpeed = InitParam.ProjectileInitSpeed;
	this->SetLifeSpan(InitParam.ProjectileLifeTime);
	ProjectileSetting = InitParam;

	if (InitParam.ProjectileStaticMesh)
	{
		BulletMesh->SetStaticMesh(InitParam.ProjectileStaticMesh);
		BulletMesh->SetRelativeTransform(InitParam.ProjectileStaticMeshOffset);
	}
	else
	{
		BulletMesh->DestroyComponent();
	}

	if (InitParam.ProjectileTrailFX)
	{
		BulletFX->SetTemplate(InitParam.ProjectileTrailFX);
		BulletFX->SetRelativeTransform(InitParam.ProjectileTrailFXOffset);
	}
	else
	{
		BulletFX->DestroyComponent();
	}

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
				UGameplayStatics::SpawnDecalAttached(myMaterial, FVector(20.0f), OtherComp, NAME_None, Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), EAttachLocation::KeepWorldPosition, 10.0f);
			}
		}
		if (ProjectileSetting.HitFXs.Contains(mySurfaceType))
		{
			UParticleSystem* myParticle = ProjectileSetting.HitFXs[mySurfaceType];
			{
				if (myParticle)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), myParticle, FTransform(Hit.ImpactNormal.Rotation(), Hit.ImpactPoint, FVector(1.0f)));
				}
			}
		}

		if (ProjectileSetting.HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSetting.HitSound, Hit.ImpactPoint);
		}
	}

	UGameplayStatics::ApplyPointDamage(OtherActor, ProjectileSetting.ProjectileDamage, Hit.TraceStart, Hit, GetInstigatorController(), this, NULL);
	UGameplayStatics::ApplyDamage(OtherActor, ProjectileSetting.ProjectileDamage, GetInstigatorController(), this, NULL);
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