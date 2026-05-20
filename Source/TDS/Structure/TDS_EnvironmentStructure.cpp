// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_EnvironmentStructure.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/ActorChannel.h"
#include "Kismet/GameplayStatics.h"

ATDS_EnvironmentStructure::ATDS_EnvironmentStructure()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

void ATDS_EnvironmentStructure::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATDS_EnvironmentStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EPhysicalSurface ATDS_EnvironmentStructure::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;
	UStaticMeshComponent* myMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (myMesh)
	{
		UMaterialInterface* myMaterial = myMesh->GetMaterial(0);
		if (myMaterial)
		{
			Result = myMaterial->GetPhysicalMaterial()->SurfaceType;
		}
	}

	return Result;
}

TArray<UTDS_StateEffect*> ATDS_EnvironmentStructure::GetAllCurrentEffects()
{
	return Effects;
}

void ATDS_EnvironmentStructure::RemoveEffect_Implementation(UTDS_StateEffect* RemoveEffect)
{
	Effects.Remove(RemoveEffect);

	SwitchEffect(RemoveEffect, false);
	EffectRemove = RemoveEffect;
}

void ATDS_EnvironmentStructure::AddEffect_Implementation(UTDS_StateEffect* newEffect)
{
	Effects.Add(newEffect);

	SwitchEffect(newEffect, true);
	EffectAdd = newEffect;
}

void ATDS_EnvironmentStructure::EffectAdd_OnRep()
{
	if (EffectAdd)
	{
		SwitchEffect(EffectAdd, true);
	}
}

void ATDS_EnvironmentStructure::EffectRemove_OnRep()
{
	if (EffectRemove)
	{
		SwitchEffect(EffectRemove, false);
	}
}

void ATDS_EnvironmentStructure::SwitchEffect(UTDS_StateEffect* Effect, bool bIsAdd)
{
	if (bIsAdd)
	{
		if (Effect && Effect->ParticleEffect)
		{
			FName NameBoneToAttached = NAME_None;
			FVector Location = OffsetEffect;

			USceneComponent* SceneComponent = GetRootComponent();

			if (SceneComponent)
			{
				UParticleSystemComponent* NewParticleSystem = UGameplayStatics::UGameplayStatics::SpawnEmitterAttached(Effect->ParticleEffect, SceneComponent,
					NameBoneToAttached, Location, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
				ParticleSystemEffects.Add(NewParticleSystem);
			}
		}
	}
	else
	{
		int32 i = 0;
		bool bIsFind = false;
		if (ParticleSystemEffects.Num() > 0)
		{
			while (i < ParticleSystemEffects.Num() && !bIsFind)
			{
				if (ParticleSystemEffects[i]->Template && Effect->ParticleEffect && Effect->ParticleEffect == ParticleSystemEffects[i]->Template)
				{
					bIsFind = true;
					ParticleSystemEffects[i]->DeactivateSystem();
					ParticleSystemEffects[i]->DestroyComponent();
					ParticleSystemEffects.RemoveAt(i);
				}
				i++;
			}
		}
	}
}

bool ATDS_EnvironmentStructure::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool Wrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (int32 i = 0; i < Effects.Num(); i++)
	{
		if (Effects[i])
		{
			Wrote |= Channel->ReplicateSubobject(Effects[i], *Bunch, *RepFlags);
		}
	}

	return Wrote;
}

void ATDS_EnvironmentStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDS_EnvironmentStructure, Effects);
	DOREPLIFETIME(ATDS_EnvironmentStructure, EffectAdd);
	DOREPLIFETIME(ATDS_EnvironmentStructure, EffectRemove);
}