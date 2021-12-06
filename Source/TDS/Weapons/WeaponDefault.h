// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h"
#include "TDS/FuncLibrary/Types.h"
#include "TDS/Weapons/Projectiles/ProjectileDefault.h"

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

	UPROPERTY()
		FWeaponInfo WeaponSetting;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
		FAdditionalWeaponInfo AdditionalWeaponInfo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Ticks
	virtual void Tick(float DeltaTime) override;

	void FireTick(float DeltaTime);
	void ReloadTick(float DeltaTime);
	void DispersionTick(float DeltaTime);
	void DropTick(float DeltaTime);
	void ShellDropTick(float DeltaTime);

	void WeaponInit();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
		bool WeaponFiring = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
		bool WeaponReloading = false;
		bool WeaponAiming = false;

	UFUNCTION(BlueprintCallable)
		void SetWeaponStateFire(bool bIsFire);

	bool CheckWeaponCanFire();

	FProjectileInfo GetProjectile();

	UFUNCTION()
	void Fire();

	void UpdateStateWeapon(EMovementState NewMovementState);
	void ChangeDispersionByShoot();
	float GetCurrentDispersion() const;
	FVector ApplyDispersionToShoot(FVector DirectionShoot) const;

	FVector GetFireEndLocation() const;
	int8 GetNumberProjectileByShoot() const;

	//timers
	float FireTimer = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic")
		float ReloadTimer = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReloadLogic Debug")//Remove
		float ReloadTimeDebug = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FireLogic")
		FName IdWeaponName;

	bool BlockFire = false;

	bool ShouldReduceDispersion = false;
	float CurrentDispersion = 0.0f;
	float CurrentDispersionMax = 1.0f;
	float CurrentDispersionMin = 0.1f;
	float CurrentDispersionRecoil = 0.1f;
	float CurrentDispersionReduction = 0.1f;

	bool DropMagasinFlag = false;
	float DropMagasinTimer = -1.0f;

	bool DropShellBulletsFlag = false;
	float DropShellBulletsTimer = -1.0f;

	FVector ShootEndLocation = FVector(0);

	UFUNCTION(BlueprintCallable)
		int32 GetWeaponRound();
	UFUNCTION()
	void InitReload();
	void FinishReload();
	void CancelReload();

	bool CheckCanWeaponReload();
	int8 GetAviableAmmoForReload();

	UFUNCTION()
		void InitDropMesh(UStaticMesh* DropMesh, FTransform Offset, FVector DropImpulseDirection, float DropTime, float LifeTimeMesh, float MassMesh, float PowerImpulse, float ImpulseRandomDispersion);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		bool ShowDebug = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
		float SizeVectorToChangeShootDirectionLogic = 100.0f;
};
