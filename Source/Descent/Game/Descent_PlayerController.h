// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Descent/Character/Descent_Character.h"
#include "GameFramework/PlayerController.h"
#include "Descent_PlayerController.generated.h"

UCLASS()
class ADescent_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	
	ADescent_PlayerController();

protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY()
	ADescent_Character* MyCharacter = nullptr;
	
	virtual void SetupInputComponent() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void InputAttackPressed();
	void InputAttackReleased();
	
	void InputSprintPressed();
	void InputSprintReleased();

	void InputAimPressed();
	void InputAimReleased();

	void ReloadWeapon();
	
	void ActiveAbility();

	void DropCurrentWeapon();

	template<int32 Id>
	void TKeyPressed();
};



