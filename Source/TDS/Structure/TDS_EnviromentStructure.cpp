// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_EnviromentStructure.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/ActorChannel.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATDS_EnviromentStructure::ATDS_EnviromentStructure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
}

// Called when the game starts or when spawned
void ATDS_EnviromentStructure::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATDS_EnviromentStructure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EPhysicalSurface ATDS_EnviromentStructure::GetSurfaceType()
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

TArray<UTDS_StateEffect*> ATDS_EnviromentStructure::GetAllCurrentEffects()
{
	return Effects;
}

void ATDS_EnviromentStructure::RemoveEffect_Implementation(UTDS_StateEffect* RemoveEffect)
{
	Effects.Remove(RemoveEffect);

	SwitchEffect(RemoveEffect, false);
	EffectRemove = RemoveEffect;
}

void ATDS_EnviromentStructure::AddEffect_Implementation(UTDS_StateEffect* newEffect)
{
	Effects.Add(newEffect);

	SwitchEffect(newEffect, true);
	EffectAdd = newEffect;
}

void ATDS_EnviromentStructure::EffectAdd_OnRep()
{
	if (EffectAdd)
	{
		SwitchEffect(EffectAdd, true);
	}
}

void ATDS_EnviromentStructure::EffectRemove_OnRep()
{
	if (EffectRemove)
	{
		SwitchEffect(EffectRemove, false);
	}
}

void ATDS_EnviromentStructure::SwitchEffect(UTDS_StateEffect* Effect, bool bIsAdd)
{
	if (bIsAdd)
	{
		if (Effect && Effect->ParticleEffect)
		{
			FName NameBoneToAttached = NAME_None;
			FVector Loc = OffsetEffect;

			USceneComponent* mySceneComp = GetRootComponent();

			if (mySceneComp)
			{
				UParticleSystemComponent* newParticleSystem = UGameplayStatics::UGameplayStatics::SpawnEmitterAttached(Effect->ParticleEffect, mySceneComp,
					NameBoneToAttached, Loc, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
				ParticleSystemEffects.Add(newParticleSystem);
			}
		}
	}
	else
	{
		int32 i = 0;
		bool bIsFind = false;
		if (ParticleSystemEffects.Num() > 0)
		{
			while (i < ParticleSystemEffects.Num(), !bIsFind)
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

bool ATDS_EnviromentStructure::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
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

void ATDS_EnviromentStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATDS_EnviromentStructure, Effects);
	DOREPLIFETIME(ATDS_EnviromentStructure, EffectAdd);
	DOREPLIFETIME(ATDS_EnviromentStructure, EffectRemove);
}