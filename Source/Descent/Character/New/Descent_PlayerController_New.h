// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Descent_PlayerController_New.generated.h"

/**
 * 
 */
UCLASS()
class DESCENT_API ADescent_PlayerController_New : public APlayerController
{
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APawn* MyPawn = nullptr;

	virtual void SetupInputComponent() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void Jump();
	void StopJump();
};
