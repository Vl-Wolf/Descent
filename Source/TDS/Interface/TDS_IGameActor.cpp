// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_IGameActor.h"

// Add default functionality here for any ITDS_IGameActor functions that are not pure virtual.

EPhysicalSurface ITDS_IGameActor::GetSurfaceType()
{
	return EPhysicalSurface::SurfaceType_Default;
}

TArray<UTDS_StateEffect*> ITDS_IGameActor::GetAllCurrentEffects()
{
	TArray<UTDS_StateEffect*> Effect;
	return Effect;
}

//void ITDS_IGameActor::RemoveEffect(UTDS_StateEffect* RemoveEffect)
//{
//
//}
//
//void ITDS_IGameActor::AddEffect(UTDS_StateEffect* newEffect)
//{
//
//}
