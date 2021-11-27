// Fill out your copyright notice in the Description page of Project Settings.


#include "TDS_StateEffect.h"
#include "TDS/Character/TDSHealthComponent.h"
#include "TDS/Interface/TDS_IGameActor.h"
#include "Kismet/GameplayStatics.h"

bool UTDS_StateEffect::InitObject(AActor* Actor)
{
	myActor = Actor;

	ITDS_IGameActor* myInterface = Cast<ITDS_IGameActor>(myActor);
	if (myInterface)
	{
		myInterface->AddEffect(this);
	}

	return true;
}

void UTDS_StateEffect::DestroyObject()
{
	ITDS_IGameActor* myInterface = Cast<ITDS_IGameActor>(myActor);
	if (myInterface)
	{
		myInterface->RemoveEffect(this);
	}

	myActor = nullptr;
	if (this && this->IsValidLowLevel())
	{
		this->ConditionalBeginDestroy();
	}
}

bool UTDS_StateEffect_ExecuteOnce::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);
	ExecuteOnce();
	return true;
}

void UTDS_StateEffect_ExecuteOnce::DestroyObject()
{
	Super::DestroyObject();
}

void UTDS_StateEffect_ExecuteOnce::ExecuteOnce()
{
	if (myActor)
	{
		UTDSHealthComponent* myHealthComponent = Cast<UTDSHealthComponent>(myActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));
		if (myHealthComponent)
		{
			myHealthComponent->ChangeHealthValue(Power);
		}
	}

	DestroyObject();
}

bool UTDS_StateEffect_ExecuteTimer::InitObject(AActor* Actor)
{
	Super::InitObject(Actor);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_EffectTimer, this, &UTDS_StateEffect_ExecuteTimer::DestroyObject, Timer, false);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ExecuteTimer, this, &UTDS_StateEffect_ExecuteTimer::Execute, RateTimer, true);
	
	if (ParticleEffect)
	{
		FName NameBoneToAttached;
		FVector Location = FVector(0);

		ParticleEmitter = UGameplayStatics::SpawnEmitterAttached(ParticleEffect, myActor->GetRootComponent(), NameBoneToAttached, Location, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
	}
	
	return true;
}


void UTDS_StateEffect_ExecuteTimer::DestroyObject()
{
	ParticleEmitter->DestroyComponent();
	ParticleEmitter = nullptr;
	Super::DestroyObject();
}

void UTDS_StateEffect_ExecuteTimer::Execute()
{
	if (myActor)
	{
		UTDSHealthComponent* myHealthComponent = Cast<UTDSHealthComponent>(myActor->GetComponentByClass(UTDSHealthComponent::StaticClass()));
		if (myHealthComponent)
		{
			myHealthComponent->ChangeHealthValue(Power);
		}
	}
}
