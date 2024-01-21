// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Descent/FuncLibrary/Types.h"
#include "ProjectileDefault.generated.h"

UCLASS()
class DESCENT_API AProjectileDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileDefault();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* BulletMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class USphereComponent* BulletCollisionSphere = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UProjectileMovementComponent* BulletProjectileMovement = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UParticleSystemComponent* BulletFX = nullptr;

	FProjectileInfo ProjectileSetting;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void InitProjectile(FProjectileInfo InitParam);
	UFUNCTION()
		virtual	void BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
		void BulletCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFroomSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void BulletCollisionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
		virtual void ImpactProjectile();

	UFUNCTION(NetMulticast, Reliable)
		void InitVisualMeshProjectile_Multicast(UStaticMesh* newMesh, FTransform MeshRelative);
	UFUNCTION(NetMulticast, Reliable)
		void InitVisualTrailProjectile_Multicast(UParticleSystem* newTrail, FTransform TrailRelative);
	UFUNCTION(NetMulticast, Reliable)
		void SpawnHitDecal_Multicast(UMaterialInterface* DecalMaterial, UPrimitiveComponent* OtherComponet, FHitResult HitResult);
	UFUNCTION(NetMulticast, Reliable)
		void SpawnHitFX_Multicast(UParticleSystem* FXTemplate, FHitResult HitResult);
	UFUNCTION(NetMulticast, Reliable)
		void SpawnHitSound_Multicast(USoundBase* HitSound, FHitResult HitResult);
	UFUNCTION(NetMulticast, Reliable)
		void InitVelocity_Multicast(float InitSpeed, float MaxSpeed);

	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

};