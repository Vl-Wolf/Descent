// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_EnviromentStructure.h"

// Sets default values
ATDS_EnviromentStructure::ATDS_EnviromentStructure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

void ATDS_EnviromentStructure::RemoveEffect(UTDS_StateEffect* RemoveEffect)
{
	Effects.Remove(RemoveEffect);
}

void ATDS_EnviromentStructure::AddEffect(UTDS_StateEffect* newEffect)
{
	Effects.Add(newEffect);
}