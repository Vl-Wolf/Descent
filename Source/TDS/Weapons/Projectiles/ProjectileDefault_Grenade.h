// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TDS/Weapons/Projectiles/ProjectileDefault.h"
#include "ProjectileDefault_Grenade.generated.h"

/**
 * 
 */
UCLASS()
class TDS_API AProjectileDefault_Grenade : public AProjectileDefault
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	void TimerExplode(float DeltaTime);

	virtual void BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void ImpactProjectile() override;

	UFUNCTION(Server, Reliable)
	void Explode_OnServer();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnExplodeFX_Multicast(UParticleSystem* FXTemplate);
	
	UFUNCTION(NetMulticast, Reliable)
	void SpawnExplodeSound_Multicast(USoundBase* ExplodeSound);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
	bool TimerEnabled = false;
	
	float TimerToExplode = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
	float TimeToExplode = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool ShowDebug = false;
};
