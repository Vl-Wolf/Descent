// Fill out your copyright notice in the Description page of Project Settings.


#include "Types.h"
#include "TDS/Interface/TDS_IGameActor.h"

void UTypes::AddEffectBySurfaceType(AActor* TakeEffectActor, FName BoneHit, TSubclassOf<UTDS_StateEffect> AddEffectClass, 
	EPhysicalSurface SurfaceType)
{
	if (SurfaceType == EPhysicalSurface::SurfaceType_Default || !TakeEffectActor || !AddEffectClass)
		return;
	
	UTDS_StateEffect* CDO = Cast<UTDS_StateEffect>(AddEffectClass->GetDefaultObject());
	if (!CDO)
		return;
	
	bool bSurfaceCompatible  = false;
	for (const auto& Surface : CDO->PossibleInteractSurface)
	{
		if (Surface == SurfaceType)
		{
			bSurfaceCompatible = true;
			break;
		}
	}
	
	if (!bSurfaceCompatible)
		return;
	
	bool bCanAdd = true;
	if (!CDO->bIsStackable)
	{
		TArray<UTDS_StateEffect*> CurrentEffects = ITDS_IGameActor::Execute_GetAllCurrentEffects(TakeEffectActor);
		
		for (UTDS_StateEffect* ExistingEffect : CurrentEffects)
		{
			if (ExistingEffect && ExistingEffect->GetClass() == AddEffectClass)
			{
				bCanAdd = false;
				break;
			}
		}
	}
	
	if (bCanAdd)
	{
		UTDS_StateEffect* NewEffect = NewObject<UTDS_StateEffect>(TakeEffectActor, AddEffectClass);
		if (NewEffect)
		{
			NewEffect->InitObject(TakeEffectActor, BoneHit);
		}
	}
}
