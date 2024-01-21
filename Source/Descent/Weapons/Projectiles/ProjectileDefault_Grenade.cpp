// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileDefault_Grenade.h"
#include "Kismet/GameplayStatics.h"
#include "Descent/Interface/Descent_IGameActor.h"
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
		TimerExplose(DeltaTime);
	}
}

void AProjectileDefault_Grenade::TimerExplose(float DeltaTime)
{
	if (TimerEnabled)
	{
		if (TimerToExplose > TimeToExplose)
		{
			Explose_OnServer();
		}
		else
		{
			TimerToExplose += DeltaTime;
		}
	}
}

void AProjectileDefault_Grenade::BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!TimerEnabled)
	{
		Explose_OnServer();
	}
	Super::BulletCollisionSphereHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AProjectileDefault_Grenade::ImpactProjectile()
{
	TimerEnabled = true;
}

void AProjectileDefault_Grenade::Explose_OnServer_Implementation()
{
	if (ShowDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSetting.ProjectileMinRadiusDamage, 12, FColor::Green, false, 12.0f);
		DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSetting.ProjectileMaxRadiusDamage, 12, FColor::Red, false, 12.0f);

	}

	TimerEnabled = false;
	if (ProjectileSetting.ExploseFX)
	{
		SpawnExploseFX_Multicast(ProjectileSetting.ExploseFX);
	}
	if (ProjectileSetting.ExploseSound)
	{
		SpawnExploseSound_Multicast(ProjectileSetting.ExploseSound);
	}

	TArray<AActor*> IgnoredActor;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		ProjectileSetting.ExploseMaxDamage,
		ProjectileSetting.ExploseMaxDamage * 0.2f,
		GetActorLocation(),
		ProjectileSetting.ProjectileMinRadiusDamage,
		ProjectileSetting.ProjectileMaxRadiusDamage, 
		ProjectileSetting.ExplodeFallCoef,
		NULL, IgnoredActor, this, nullptr);

	this->Destroy();
}

void AProjectileDefault_Grenade::SpawnExploseFX_Multicast_Implementation(UParticleSystem* FXTemplate)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FXTemplate, GetActorLocation(), GetActorRotation(), FVector(1.0f));
}

void AProjectileDefault_Grenade::SpawnExploseSound_Multicast_Implementation(USoundBase* ExploseSound)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExploseSound, GetActorLocation());
}
