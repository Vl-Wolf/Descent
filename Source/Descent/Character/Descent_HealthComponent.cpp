// Fill out your copyright notice in the Description page of Project Settings.


#include "Descent_HealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UDescent_HealthComponent::UDescent_HealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UDescent_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDescent_HealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UDescent_HealthComponent::GetCurrentHealth()
{
	return Health;
}

void UDescent_HealthComponent::SetCurrentHealth(float NewHealth)
{
	Health = NewHealth;
}

bool UDescent_HealthComponent::GetIsAlive()
{
	return bIsAlive;
}

void UDescent_HealthComponent::ChangeHealthValue_OnServer_Implementation(float ChangeValue)
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

void UDescent_HealthComponent::HealthChangeEvent_Multicast_Implementation(float newHealth, float value)
{
	OnHealthChange.Broadcast(newHealth, value);
}

void UDescent_HealthComponent::DeadEvent_Multicast_Implementation()
{
	OnDead.Broadcast();
}

void UDescent_HealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDescent_HealthComponent, Health);
	DOREPLIFETIME(UDescent_HealthComponent, bIsAlive);
}

