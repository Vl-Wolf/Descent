// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Descent_GameMode.generated.h"

UCLASS(minimalapi)
class ADescent_GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADescent_GameMode();

	void PlayerDead();
};



