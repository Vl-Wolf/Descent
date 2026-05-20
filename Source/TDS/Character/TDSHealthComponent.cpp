// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSHealthComponent.h"
#include "Net/UnrealNetwork.h"

UTDSHealthComponent::UTDSHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
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
		ChangeValue = ChangeValue * CoefficientDamage;

		Health += ChangeValue;

		HealthChangeEvent_Multicast(Health, ChangeValue);

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

