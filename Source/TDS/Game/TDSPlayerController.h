// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TDSPlayerController.generated.h"

UCLASS()
class ATDSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATDSPlayerController();

protected:

	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface
	
	virtual void OnUnPossess() override;
};


