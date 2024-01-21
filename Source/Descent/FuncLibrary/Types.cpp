// Fill out your copyright notice in the Description page of Project Settings.


#include "Types.h"
//#include "Descent.h"
#include "Descent/Interface/Descent_IGameActor.h"

void UTypes::AddEffectBySurfaceType(AActor* TakeEffectActor, FName BoneHit, TSubclassOf<UDescent_StateEffect> AddEffectClass, EPhysicalSurface SurfaceType)
{
	if (SurfaceType != EPhysicalSurface::SurfaceType_Default && TakeEffectActor && AddEffectClass)
	{
		UDescent_StateEffect* myEffect = Cast<UDescent_StateEffect>(AddEffectClass->GetDefaultObject());
		if (myEffect)
		{
			bool bIsHavePossibleSurface = false;
			int8 i = 0;
			while (i < myEffect->PossibleInteractSurface.Num() && !bIsHavePossibleSurface)
			{
				if (myEffect->PossibleInteractSurface[i] == SurfaceType)
				{
					bIsHavePossibleSurface = true;
					bool bIsCanAddEffect = false;
					if (!myEffect->bIsStakable)
					{
						int8 j = 0;
						TArray<UDescent_StateEffect*> CurrentEffects;
						IDescent_IGameActor* myInterface = Cast<IDescent_IGameActor>(TakeEffectActor);
						if (myInterface)
						{
							CurrentEffects = myInterface->GetAllCurrentEffects();
						}

						if (CurrentEffects.Num() > 0)
						{
							while (j < CurrentEffects.Num() && !bIsCanAddEffect)
							{
								if (CurrentEffects[j]->StaticClass() != AddEffectClass)
								{
									bIsCanAddEffect = true;
								}
								j++;
							}
						}
						else
						{
							bIsCanAddEffect = true;
						}

					}
					else
					{
						bIsCanAddEffect = true;
					}

					if (bIsCanAddEffect)
					{

						UDescent_StateEffect* NewEffect = NewObject<UDescent_StateEffect>(TakeEffectActor, AddEffectClass);
						if (NewEffect)
						{
							NewEffect->InitObject(TakeEffectActor, BoneHit);
						}
					}

				}
				i++;
			}
		}

	}
}
