// Fill out your copyright notice in the Description page of Project Settings.


#include "Descent_PlayerController_New.h"

void ADescent_PlayerController_New::BeginPlay()
{
	Super::BeginPlay();

	MyPawn = GetPawn();
}

void ADescent_PlayerController_New::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &ADescent_PlayerController_New::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ADescent_PlayerController_New::MoveRight);
	InputComponent->BindAxis("Turn", this, &ADescent_PlayerController_New::Turn);
	InputComponent->BindAxis("LookUp", this, &ADescent_PlayerController_New::LookUp);

	//InputComponent->BindAction("", this, &ADescent_PlayerController_New::LookUp);
}

void ADescent_PlayerController_New::MoveForward(float Value)
{
	if (Value != 0.0f && MyPawn)
	{
		const FRotator PawnControlRotation = GetControlRotation();
		const FVector ForwardVector = FRotationMatrix(PawnControlRotation).GetScaledAxis(EAxis::X);
		MyPawn->AddMovementInput(ForwardVector, Value);
		MyPawn->bUseControllerRotationYaw = true;
	}
}

void ADescent_PlayerController_New::MoveRight(float Value)
{
	if (Value != 0.0f && MyPawn)
	{
		const FRotator PawnControlRotation = GetControlRotation();
		const FVector RightVector = FRotationMatrix(PawnControlRotation).GetScaledAxis(EAxis::Y);
		MyPawn->AddMovementInput(RightVector, Value);
		MyPawn->bUseControllerRotationYaw = true;
	}
}

void ADescent_PlayerController_New::Turn(float Value)
{
	AddYawInput(Value);
}

void ADescent_PlayerController_New::LookUp(float Value)
{
	AddPitchInput(Value);
}

void ADescent_PlayerController_New::Jump()
{
}

void ADescent_PlayerController_New::StopJump()
{
}
