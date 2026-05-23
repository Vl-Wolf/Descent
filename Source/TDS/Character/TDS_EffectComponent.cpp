// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_EffectComponent.h"

#include "Engine/ActorChannel.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UTDS_EffectComponent::UTDS_EffectComponent()
{
	SetIsReplicatedByDefault(true);
}

void UTDS_EffectComponent::ApplyEffect(UTDS_StateEffect* NewEffect)
{
	if (!NewEffect || ActiveEffects.Contains(NewEffect))
		return;
	
	ActiveEffects.Add(NewEffect);
	ToggleEffectVisuals(NewEffect, true);
	LastAddedEffect = NewEffect;
	
}

void UTDS_EffectComponent::RemoveEffect(UTDS_StateEffect* NewEffect)
{
	if (!NewEffect)
		return;
	
	ActiveEffects.Remove(NewEffect);
	ToggleEffectVisuals(NewEffect, false);
	LastRemovedEffect = NewEffect;
}

TArray<UTDS_StateEffect*> UTDS_EffectComponent::GetActiveEffects() const
{
	return ActiveEffects;
}

void UTDS_EffectComponent::SetAttachTarget(USceneComponent* InTarget, FVector InOffset)
{
	AttachTarget = InTarget;
	AttachOffset = InOffset;
}

bool UTDS_EffectComponent::ReplicateEffectSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
	FReplicationFlags* RepFlags)
{
	bool Wrote = false;
	for (UTDS_StateEffect* Effect : ActiveEffects)
	{
		if (Effect)
		{
			Wrote |= Channel->ReplicateSubobject(Effect, *Bunch, *RepFlags);
		}
	}
	return Wrote;
}

void UTDS_EffectComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UTDS_EffectComponent, ActiveEffects);
	DOREPLIFETIME(UTDS_EffectComponent, LastAddedEffect);
	DOREPLIFETIME(UTDS_EffectComponent, LastRemovedEffect);
}

void UTDS_EffectComponent::EffectAdded_OnRep()
{
	if (LastAddedEffect)
		ToggleEffectVisuals(LastAddedEffect, true);
}

void UTDS_EffectComponent::EffectRemoved_OnRep()
{
	if (LastRemovedEffect)
		ToggleEffectVisuals(LastRemovedEffect, false);
}

void UTDS_EffectComponent::ToggleEffectVisuals(UTDS_StateEffect* Effect, bool bIsAdd)
{
	if (bIsAdd)
	{
		if (!Effect || !Effect->ParticleEffect || !AttachTarget)
			return;

		UParticleSystemComponent* NewPS = UGameplayStatics::SpawnEmitterAttached(
			Effect->ParticleEffect,
			AttachTarget,
			Effect->NameBone,
			AttachOffset,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			false);

		ParticleSystemComponents.Add(NewPS);
	}
	else
	{
		for (int32 i = ParticleSystemComponents.Num() - 1; i >= 0; --i)
		{
			if (ParticleSystemComponents[i]
				&& ParticleSystemComponents[i]->Template == Effect->ParticleEffect)
			{
				ParticleSystemComponents[i]->DeactivateSystem();
				ParticleSystemComponents[i]->DestroyComponent();
				ParticleSystemComponents.RemoveAt(i);
				break;
			}
		}
	}
}


