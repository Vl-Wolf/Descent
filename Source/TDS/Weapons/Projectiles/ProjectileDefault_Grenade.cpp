// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileDefault_Grenade.h"
#include "Kismet/GameplayStatics.h"
#include "TDS/Interface/TDS_IGameActor.h"
#include "DrawDebugHelpers.h"

void AProjectileDefault_Grenade::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectileDefault_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		TimerExplode(DeltaTime);
	}
}

void AProjectileDefault_Grenade::TimerExplode(float DeltaTime)
{
	if (TimerEnabled)
	{
		if (TimerToExplode > TimeToExplode)
		{
			Explode_OnServer();
		}
		else
		{
			TimerToExplode += DeltaTime;
		}
	}
}

void AProjectileDefault_Grenade::BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!TimerEnabled)
	{
		Explode_OnServer();
	}
	Super::BulletCollisionSphereHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileDefault_Grenade::ImpactProjectile()
{
	TimerEnabled = true;
}

void AProjectileDefault_Grenade::Explode_OnServer_Implementation()
{
	if (ShowDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSetting.ProjectileMinRadiusDamage, 12, FColor::Green, false, 12.0f);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSetting.ProjectileMaxRadiusDamage, 12, FColor::Red, false, 12.0f);

	}

	TimerEnabled = false;
	if (ProjectileSetting.ExplodeFX)
	{
		SpawnExplodeFX_Multicast(ProjectileSetting.ExplodeFX);
	}
	if (ProjectileSetting.ExplodeSound)
	{
		SpawnExplodeSound_Multicast(ProjectileSetting.ExplodeSound);
	}

	TArray<AActor*> IgnoredActor;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		ProjectileSetting.ExplodeMaxDamage,
		ProjectileSetting.ExplodeMaxDamage * 0.2f,
		GetActorLocation(),
		ProjectileSetting.ProjectileMinRadiusDamage,
		ProjectileSetting.ProjectileMaxRadiusDamage, 
		ProjectileSetting.ExplodeFallCoef,
		NULL, IgnoredActor, this, nullptr);

	this->Destroy();
}

void AProjectileDefault_Grenade::SpawnExplodeFX_Multicast_Implementation(UParticleSystem* FXTemplate)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FXTemplate, GetActorLocation(), GetActorRotation(), FVector(1.0f));
}

void AProjectileDefault_Grenade::SpawnExplodeSound_Multicast_Implementation(USoundBase* ExplodeSound)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplodeSound, GetActorLocation());
}
