// Fill out your copyright notice in the Description page of Project Settings.


#include "Descent_CharacterHealthComponent.h"

void UDescent_CharacterHealthComponent::ChangeHealthValue_OnServer(float ChangeValue)
{
	float CurrentDamage = ChangeValue * CoefDamage;

	if (Shield > 0.0f && ChangeValue < 0.0f)
	{
		ChangeShieldValue(ChangeValue);
		if (Shield < 0.0f)
		{
			//fx
		}
	}
	else
	{
		Super::ChangeHealthValue_OnServer(ChangeValue);
	}
}

float UDescent_CharacterHealthComponent::GetCurrentShield()
{
	return Shield;
}

void UDescent_CharacterHealthComponent::ChangeShieldValue(float ChangeValue)
{
	Shield += ChangeValue;

	ShieldChangeEvent_Multicast(Shield, ChangeValue);
	//OnShieldChange.Broadcast(Shield, ChangeValue);

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
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CoolDownShieldTimer, this, &UDescent_CharacterHealthComponent::CoolDownShieldEnd, CoolDownShieldRecoveryTime, false);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldRecoveryRateTimer);
	}

}

void UDescent_CharacterHealthComponent::CoolDownShieldEnd()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShieldRecoveryRateTimer, this, &UDescent_CharacterHealthComponent::RecoveryShield, ShieldRecoveryRate, true);
	}
}

void UDescent_CharacterHealthComponent::RecoveryShield()
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
	//OnShieldChange.Broadcast(Shield, ShieldRecoveryValue);
}

float UDescent_CharacterHealthComponent::GetShieldValue()
{
	return Shield;
}

void UDescent_CharacterHealthComponent::ShieldChangeEvent_Multicast_Implementation(float newShield, float Damage)
{
	OnShieldChange.Broadcast(newShield, Damage);
}
