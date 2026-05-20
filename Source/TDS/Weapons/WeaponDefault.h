// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "TDS/FuncLibrary/Types.h"

#include "WeaponDefault.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponReloadStart, UAnimMontage*, AnimReloadStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponReloadEnd, bool, bIsSuccess, int32, AmmoSafe);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponFire, UAnimMontage*, AnimFireChar);

UCLASS()
class TDS_API AWeaponDefault : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponDefault();

	FOnWeaponReloadStart OnWeaponReloadStart;
	FOnWeaponReloadEnd OnWeaponReloadEnd;
	FOnWeaponFire OnWeaponFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	class USceneComponent* SceneComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	class USkeletalMeshComponent* SkeletalMeshWeapon = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	class UStaticMeshComponent* StaticMeshWeapon = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
	class UArrowComponent* ShootLocation = nullptr;

	UPROPERTY(VisibleAnywhere)
	FWeaponInfo WeaponSetting;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	FAdditionalWeaponInfo AdditionalWeaponInfo;

	//Timers
	FTimerHandle FireTimerHandle;
	FTimerHandle ReloadTimerHandle;
	FTimerHandle DropMagazineTimerHandle;
	FTimerHandle ShellDropTimerHandle;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
	float ReloadTime = 0.0f;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
	FName IdWeaponName;

	//Flags
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
	bool WeaponFiring = false;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
	bool WeaponReloading = false;
	bool WeaponAiming = false;
	bool BlockFire = false;

	//Dispersion
	bool ShouldReduceDispersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 0.1f;
	float CurrentDispersionReduction = 0.1f;
	
	UPROPERTY(Replicated)
	FVector ShootEndLocation = FVector(0);

protected:

	virtual void BeginPlay() override;
	
	void DispersionTick(float DeltaTime);

	void WeaponInit();

	UFUNCTION()
	void Fire();

public:	
	// Ticks
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void SetWeaponStateFire_OnServer(bool bIsFire);

	bool CheckWeaponCanFire();

	FProjectileInfo GetProjectile();

	UFUNCTION(Server, Reliable)
	void UpdateStateWeapon_OnServer(EMovementState NewMovementState);
	
	void ChangeDispersionByShoot();
	
	float GetCurrentDispersion() const;
	
	FVector ApplyDispersionToShoot(FVector DirectionShoot) const;

	FVector GetFireEndLocation() const;
	
	int8 GetNumberProjectileByShoot() const;

	UFUNCTION(BlueprintCallable)
	int32 GetWeaponRound();

	UFUNCTION()
	void InitReload();
	void FinishReload();
	void CancelReload();

	bool CheckCanWeaponReload();
	int8 GetAviableAmmoForReload();

	void DropMagazine();
	
	void DropShell();
	
	UFUNCTION(Server, Reliable)
	void InitDropMesh_OnServer(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float DropTime, float LifeTimeMesh, float MassMesh, float PowerImpulse, float ImpulseRandomDispersion);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool ShowDebug = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float SizeVectorToChangeShootDirectionLogic = 100.0f;


	//Network
	UFUNCTION(Server, Unreliable)
	void UpdateWeaponByCharacterMovementState_OnServer(FVector NewShootEndLocation, bool NewShouldReduceDispersion);
	
	UFUNCTION(NetMulticast, Unreliable)
	void AnimWeaponStart_Multicast(UAnimMontage* Anim);
	
	UFUNCTION(NetMulticast, Unreliable)
	void ShellDropFire_Multicast(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float DropTime, float LifeTimeMesh, float MassMesh, float PowerImpulse, float ImpulseRandomDispersion, FVector LocalDir);
	
	UFUNCTION(NetMulticast, Unreliable)
	void FXWeaponFire_Multicast(UParticleSystem* FXFire, USoundBase* SoundFire);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnTraceHitDecal_Multicast(UMaterialInterface* DecalMaterial, FHitResult HitResult);
	
	UFUNCTION(NetMulticast, Reliable)
	void SpawnTraceHitFX_Multicast(UParticleSystem* FXTemplate, FHitResult HitResult);
	
	UFUNCTION(NetMulticast, Reliable)
	void SpawnTraceHitSound_Multicast(USoundBase* HitSound, FHitResult HitResult);

};
