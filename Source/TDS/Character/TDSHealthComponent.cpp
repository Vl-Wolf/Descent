// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UTDSHealthComponent::UTDSHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UTDSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTDSHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UTDSHealthComponent::GetCurrentHealth()
{
	return Health;
}

void UTDSHealthComponent::SetCurrentHealth(float NewHealth)
{
	Health = NewHealth;
}

bool UTDSHealthComponent::GetIsAlive()
{
	return bIsAlive;
}

void UTDSHealthComponent::ChangeHealthValue_OnServer_Implementation(float ChangeValue)
{

	if (bIsAlive)
	{
		ChangeValue = ChangeValue * CoefDamage;

		Health += ChangeValue;

		HealthChangeEvent_Multicast(Health, ChangeValue);
		//OnHealthChange.Broadcast(Health, ChangeValue);

		if (Health > 100.0f)
		{
			Health = 100.0f;
		}
		else
		{
			if (Health < 0.0f)
			{
				bIsAlive = false;
				DeadEvent_Multicast();
				//OnDead.Broadcast();
			}
		}
	}
	
}

void UTDSHealthComponent::HealthChangeEvent_Multicast_Implementation(float newHealth, float value)
{
	OnHealthChange.Broadcast(newHealth, value);
}

void UTDSHealthComponent::DeadEvent_Multicast_Implementation()
{
	OnDead.Broadcast();
}

void UTDSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTDSHealthComponent, Health);
	DOREPLIFETIME(UTDSHealthComponent, bIsAlive);
}

