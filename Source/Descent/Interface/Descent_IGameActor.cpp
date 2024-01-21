// Fill out your copyright notice in the Description page of Project Settings.


#include "Descent_IGameActor.h"

// Add default functionality here for any IDescent_IGameActor functions that are not pure virtual.

EPhysicalSurface IDescent_IGameActor::GetSurfaceType()
{
	return EPhysicalSurface::SurfaceType_Default;
}

TArray<UDescent_StateEffect*> IDescent_IGameActor::GetAllCurrentEffects()
{
	TArray<UDescent_StateEffect*> Effect;
	return Effect;
}

//void IDescent_IGameActor::RemoveEffect(UDescent_StateEffect* RemoveEffect)
//{
//
//}
//
//void IDescent_IGameActor::AddEffect(UDescent_StateEffect* newEffect)
//{
//
//}
