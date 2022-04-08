// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TDS/StateEffects/TDS_StateEffect.h"
#include "TDS/FuncLibrary/Types.h"
#include "TDS_IGameActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTDS_IGameActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TDS_API ITDS_IGameActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual EPhysicalSurface GetSurfaceType();

	virtual TArray<UTDS_StateEffect*> GetAllCurrentEffects();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveEffect(UTDS_StateEffect* RemoveEffect);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddEffect(UTDS_StateEffect* newEffect);

	//inv
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void DropWeaponToWorld(FDropItem DropItemInfo);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void DropAmmoToWorld(EWeaponType TypeAmmo, int32 Cout);
};
