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
	
	EffectComponent = CreateDefaultSubobject<UTDS_EffectComponent>(TEXT("EffectComponent"));
}

void ATDS_EnvironmentStructure::BeginPlay()
{
	Super::BeginPlay();
	
	EffectComponent->SetAttachTarget(GetRootComponent(), OffsetEffect);
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

TArray<UTDS_StateEffect*> ATDS_EnvironmentStructure::GetAllCurrentEffects_Implementation()
{
	return EffectComponent->GetActiveEffects();
}

void ATDS_EnvironmentStructure::RemoveEffect_Implementation(UTDS_StateEffect* RemoveEffect)
{
	EffectComponent->RemoveEffect(RemoveEffect);
}

void ATDS_EnvironmentStructure::AddEffect_Implementation(UTDS_StateEffect* NewEffect)
{
	EffectComponent->ApplyEffect(NewEffect);

}

bool ATDS_EnvironmentStructure::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool Wrote = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
	Wrote |= EffectComponent->ReplicateEffectSubobjects(Channel, Bunch, RepFlags);
	return Wrote;
}

void ATDS_EnvironmentStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}