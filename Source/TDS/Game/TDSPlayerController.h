// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TDS/Character/TDSCharacter.h"
#include "TDS/FuncLibrary/Types.h"
#include "TDSPlayerController.generated.h"

UCLASS()
class ATDSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	ATDSPlayerController();

protected:
	
	virtual void PlayerTick(float DeltaTime) override;
	
	virtual void SetupInputComponent() override;
	
	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void OnUnPossess() override;
	
private:
	
	void InputAxisX(float Value);
	void InputAxisY(float Value);
	
	void InputAttackPressed();
	void InputAttackReleased();

	void InputWalkPressed();
	void InputWalkReleased();

	void InputSprintPressed();
	void InputSprintReleased();

	void InputAimPressed();
	void InputAimReleased();
	
	void InputReload();
	void InputAbility();
	void InputDropWeapon();
	
	void InputSwitchNextWeapon();
	void InputSwitchPreviousWeapon();
	
	template<int32 Id>
	void InputSwitchToSlot()
	{
		ATDSCharacter* Char = Cast<ATDSCharacter>(GetPawn());
		if (Char)
			Char->RequestSwitchWeaponByIndex(Id);
	}
	
	float AxisX = 0.0f;
	float AxisY = 0.0f;

	bool bIsSprintEnabled = false;
	bool bIsWalkEnabled = false;
	bool bIsAimEnabled = false;
	
	EMovementState DetermineMovementState() const;
	
	UPROPERTY()
	UDecalComponent* CursorDecal = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor", meta=(AllowPrivateAccess = "true"))
	UMaterialInterface* CursorMaterial = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor", meta=(AllowPrivateAccess = "true"))
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);
	
	float LookAxisX = 0.0f;
	float LookAxisY = 0.0f;
	
	bool bIsGamepadActive = false;
	
	void OnLookRight(float Value);
	void OnLookUp(float Value);
	
	FVector VirtualCursorLocation = FVector::ZeroVector;
	
	UPROPERTY(EditDefaultsOnly, Category = "Cursor")
	float VirtualCursorRadius = 500.f;
	
};


