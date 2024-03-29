// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Descent/StateEffects/Descent_StateEffect.h"
#include "Descent/FuncLibrary/Types.h"
#include "Descent_IGameActor.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDescent_IGameActor : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DESCENT_API IDescent_IGameActor
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual EPhysicalSurface GetSurfaceType();

	virtual TArray<UDescent_StateEffect*> GetAllCurrentEffects();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RemoveEffect(UDescent_StateEffect* RemoveEffect);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void AddEffect(UDescent_StateEffect* newEffect);

	//inv
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void DropWeaponToWorld(FDropItem DropItemInfo);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void DropAmmoToWorld(EWeaponType TypeAmmo, int32 Cout);
};
