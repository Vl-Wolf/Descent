// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TDS/FuncLibrary/Types.h"
#include "TDS/Weapons/WeaponDefault.h"
#include "TDS/Character/TDSInventoryComponent.h"
#include "TDS/Character/TDSCharacterHealthComponent.h"
#include "TDS/Character/TDS_EffectComponent.h"
#include "TDS/Interface/TDS_IGameActor.h"
#include "TDS/StateEffects/TDS_StateEffect.h"
#include "TDSCharacter.generated.h"

UCLASS(Blueprintable)
class ATDSCharacter : public ACharacter, public ITDS_IGameActor
{
	GENERATED_BODY()

protected:

	bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	virtual void BeginPlay() override;
	
	void TrySwitchNextWeapon();
	void TrySwitchPreviousWeapon();
	
	void TryAbilityEnabled();
	
	UFUNCTION(Server, Reliable)
	void TryAbilityEnabled_OnServer();	
	
	UPROPERTY(Replicated)
	EMovementState MovementState = EMovementState::Run_State;
	
	UPROPERTY(Replicated)
	AWeaponDefault* CurrentWeapon = nullptr;
		
	UPROPERTY(Replicated)
	int32 CurrentIndexWeapon = 0;

	UFUNCTION()
	void CharacterDead();
	
	UFUNCTION(NetMulticast, Reliable)
	void EnableRagdoll_Multicast();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
public:
	
	ATDSCharacter();

	FTimerHandle TimerHandle_RagDollTimer;
	
	virtual void Tick(float DeltaSeconds) override;
	
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UTDSInventoryComponent* InventoryComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health", meta = (AllowPrivateAccess = "true"))
	class UTDSCharacterHealthComponent* HealthComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Effect", meta = (AllowPrivateAccess = "true"))
	class UTDS_EffectComponent* EffectComponent;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	FCharacterSpeed MovementInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death")
	TArray<UAnimMontage*> DeadsAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TSubclassOf<UTDS_StateEffect> AbilityEffect;

private:

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:
		
	void CharacterUpdate();
	
	void AttackCharEvent(bool bIsFiring);

	void RequestSwitchWeaponByIndex(int32 Id);
	
	void SetAimLocation(FVector Location);
	
	void SetFiring(bool bIsFiring);
	
	void RequestReload();
	void RequestAbility();
	void RequestDropWeapon();
	
	void RequestSwitchNextWeapon();
	void RequestSwitchPreviousWeapon();
		
	UFUNCTION()
	void InitWeapon(FName IdWeaponName, FAdditionalWeaponInfo WeaponAdditionalInfo, int32 NewCurrentIndexWeapon);
	
	void TryReloadWeapon();

	UFUNCTION()
	void WeaponFire(UAnimMontage* Anim);
	
	UFUNCTION()
	void WeaponReloadStart(UAnimMontage* Anim);
	
	UFUNCTION()
	void WeaponReloadEnd(bool bIsSuccess, int32 AmmoSafe);
	
	UFUNCTION(Server, Reliable)
	void TrySwitchWeaponToIndexByKeyInput_OnServer(int32 ToIndex);
	
	void DropCurrentWeapon();

	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadStart_BP(UAnimMontage* Anim);
	
	UFUNCTION(BlueprintNativeEvent)
	void WeaponReloadEnd_BP(bool bIsSuccess);
	
	UFUNCTION(BlueprintNativeEvent)
	void WeaponFire_BP(UAnimMontage* Anim);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AWeaponDefault* GetCurrentWeapon();
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EMovementState GetMovementState();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UTDS_StateEffect*> GetCurrentEffectsOnChar();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetCurrentWeaponIndex();
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsAlive();
	
	EPhysicalSurface GetSurfaceType() override;

	virtual TArray<UTDS_StateEffect*> GetAllCurrentEffects_Implementation() override;

	virtual void RemoveEffect_Implementation(UTDS_StateEffect* RemoveEffect) override;
	
	virtual void AddEffect_Implementation(UTDS_StateEffect* NewEffect) override;

	UFUNCTION(BlueprintNativeEvent)
	void CharacterDead_BP();

	UFUNCTION(Server, Unreliable)
	void SetActorRotationByYaw_OnServer(float Yaw);
	
	UFUNCTION(NetMulticast, Unreliable)
	void SetActorRotationByYaw_Multicast(float Yaw);

	UFUNCTION(Server, Reliable)
	void SetMovementState_OnServer(EMovementState NewState);
	
	UFUNCTION(NetMulticast, Reliable)
	void SetMovementState_Multicast(EMovementState NewState);
	
	UFUNCTION(Server, Reliable)
	void TryReloadWeapon_OnServer();
	
	UFUNCTION(NetMulticast, Reliable)
	void PlayAnim_Multicast(UAnimMontage* Anim);
	
};

