// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSCharacterHealthComponent.h"

void UTDSCharacterHealthComponent::ChangeHealthValue_OnServer(float ChangeValue)
{
	if (Shield > 0.0f && ChangeValue < 0.0f)
	{
		const float OverflowDamage = Shield + ChangeValue;
		
		ChangeShieldValue(ChangeValue);
		if (Shield <= 0.0f)
		{
			Super::ChangeHealthValue_OnServer(OverflowDamage);
		}
	}
	else
	{
		Super::ChangeHealthValue_OnServer(ChangeValue);
	}
}

float UTDSCharacterHealthComponent::GetCurrentShield()
{
	return Shield;
}

void UTDSCharacterHealthComponent::ChangeShieldValue(float ChangeValue)
{
	Shield += ChangeValue;

	ShieldChangeEvent_Multicast(Shield, ChangeValue);

	if (Shield > 100.0f)
	{
		Shield = 100.0f;
	}
	else
	{
		if (Shield < 0.0f)
		{
			Shield = 0.0f;
		}
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CoolDownShieldTimer, this, &UTDSCharacterHealthComponent::CoolDownShieldEnd, CoolDownShieldRecoveryTime, false);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecoveryRateTimer);
	}

}

void UTDSCharacterHealthComponent::CoolDownShieldEnd()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShieldRecoveryRateTimer, this, &UTDSCharacterHealthComponent::RecoveryShield, ShieldRecoveryRate, true);
	}
}

void UTDSCharacterHealthComponent::RecoveryShield()
{
	float tmp = Shield;
	tmp += ShieldRecoveryValue;

	if (tmp > 100.f)
	{
		Shield = 100.0f;

		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CoolDownShieldTimer);
		}
	}
	else
	{
		Shield = tmp;
	}

	ShieldChangeEvent_Multicast(Shield, ShieldRecoveryValue);
}

float UTDSCharacterHealthComponent::GetShieldValue()
{
	return Shield;
}

void UTDSCharacterHealthComponent::ShieldChangeEvent_Multicast_Implementation(float NewShield, float Damage)
{
	OnShieldChange.Broadcast(NewShield, Damage);
}
