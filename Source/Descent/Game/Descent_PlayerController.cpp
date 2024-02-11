// Copyright Epic Games, Inc. All Rights Reserved.

#include "Descent_PlayerController.h"
#include "Descent/Character/Descent_Character.h"
#include "Engine/World.h"

ADescent_PlayerController::ADescent_PlayerController()
{
	bShowMouseCursor = false;
	bEnableTouchEvents = false;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ADescent_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	MyCharacter = Cast<ADescent_Character>(GetPawn());
}


void ADescent_PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &ADescent_PlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ADescent_PlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &ADescent_PlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &ADescent_PlayerController::LookUp);
	
	InputComponent->BindAction("ChangeToSprint", IE_Pressed, this, &ADescent_PlayerController::InputSprintPressed);
	InputComponent->BindAction("ChangeToSprint", IE_Released, this, &ADescent_PlayerController::InputSprintReleased);

	InputComponent->BindAction("AimEvent", IE_Pressed, this, &ADescent_PlayerController::InputAimPressed);
	InputComponent->BindAction("AimEvent", IE_Released, this, &ADescent_PlayerController::InputAimReleased);

	InputComponent->BindAction("FireEvent", IE_Pressed, this, &ADescent_PlayerController::InputAttackPressed);
	InputComponent->BindAction("FireEvent", IE_Released, this, &ADescent_PlayerController::InputAttackReleased);

	InputComponent->BindAction("ReloadEvent", IE_Released, this, &ADescent_PlayerController::ReloadWeapon);

	InputComponent->BindAction("AbilityAction", IE_Pressed, this, &ADescent_PlayerController::ActiveAbility);

	InputComponent->BindAction("DropCurrentWeapon", IE_Pressed, this, &ADescent_PlayerController::DropCurrentWeapon);

	TArray<FKey> HotKeys;
	HotKeys.Add(EKeys::One);
	HotKeys.Add(EKeys::Two);
	HotKeys.Add(EKeys::Three);
	HotKeys.Add(EKeys::Four);
	InputComponent->BindKey(HotKeys[0], IE_Pressed, this, &ADescent_PlayerController::TKeyPressed<0>);
	InputComponent->BindKey(HotKeys[1], IE_Pressed, this, &ADescent_PlayerController::TKeyPressed<1>);
	InputComponent->BindKey(HotKeys[2], IE_Pressed, this, &ADescent_PlayerController::TKeyPressed<2>);
	InputComponent->BindKey(HotKeys[3], IE_Pressed, this, &ADescent_PlayerController::TKeyPressed<3>);
	
}

void ADescent_PlayerController::MoveForward(float Value)
{
	MyCharacter->AxisX = Value;
}

void ADescent_PlayerController::MoveRight(float Value)
{
	MyCharacter->AxisY = Value;
}

void ADescent_PlayerController::Turn(float Value)
{
	AddYawInput(Value);
}

void ADescent_PlayerController::LookUp(float Value)
{
	AddPitchInput(Value);
}

void ADescent_PlayerController::InputAttackPressed()
{
	if (MyCharacter->HealthComponent && MyCharacter->HealthComponent->GetIsAlive())
	{
		MyCharacter->AttackCharEvent(true);
	}
}

void ADescent_PlayerController::InputAttackReleased()
{
	MyCharacter->AttackCharEvent(false);
}

void ADescent_PlayerController::InputSprintPressed()
{
	MyCharacter->SprintRunEnabled = true;
	MyCharacter->ChangeMovementState();
}

void ADescent_PlayerController::InputSprintReleased()
{
	MyCharacter->SprintRunEnabled = false;
	MyCharacter->ChangeMovementState();
}

void ADescent_PlayerController::InputAimPressed()
{
	MyCharacter->AimEnabled = true;
	MyCharacter->ChangeMovementState();
}

void ADescent_PlayerController::InputAimReleased()
{
	MyCharacter->AimEnabled = false;
	MyCharacter->ChangeMovementState();
}

void ADescent_PlayerController::ReloadWeapon()
{
	if (MyCharacter->HealthComponent && MyCharacter->HealthComponent->GetIsAlive() && MyCharacter->CurrentWeapon && !MyCharacter->CurrentWeapon->WeaponReloading)
	{
		MyCharacter->TryReloadWeapon();
	}
}

void ADescent_PlayerController::ActiveAbility()
{
	MyCharacter->TryAbilityEnabled();
}

void ADescent_PlayerController::DropCurrentWeapon()
{
	MyCharacter->DropCurrentWeapon();
}

template <int32 Id>
void ADescent_PlayerController::TKeyPressed()
{
	MyCharacter->TrySwitchWeaponToIndexByKeyInput_OnServer(Id);
}

