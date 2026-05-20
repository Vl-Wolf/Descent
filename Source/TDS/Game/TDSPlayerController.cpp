// Copyright Epic Games, Inc. All Rights Reserved.

#include "TDSPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "TDS/Character/TDSCharacter.h"
#include "Engine/World.h"

ATDSPlayerController::ATDSPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ATDSPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void ATDSPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();
}

void ATDSPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}
