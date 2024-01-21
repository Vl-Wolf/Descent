// Fill out your copyright notice in the Description page of Project Settings.


#include "Descent_GameInstance.h"

bool UDescent_GameInstance::GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo)
{
	bool bIsFind = false;
	FWeaponInfo* WeaponInfoRow;

	if (WeaponInfoTable)
	{
		WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(NameWeapon, "", false);
		if (WeaponInfoRow)
		{
			bIsFind = true;
			OutInfo = *WeaponInfoRow;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDescent_GameInstance::GetWeaponInfoByName - WeaponTable -NULL"));
	}

	return bIsFind;
}

bool UDescent_GameInstance::GetDropItemInfoByWeaponName(FName NameItem, FDropItem& OutInfo)
{
	bool bIsFind = false;

	if (DropItemInfoTable)
	{
		FDropItem* DropItemInfoRow;
		TArray<FName>RowNames = DropItemInfoTable->GetRowNames();
		int8 i = 0;
		while (i < RowNames.Num() && !bIsFind)
		{
			DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(RowNames[i], "");
			if (DropItemInfoRow->WeaponInfo.NameItem == NameItem)
			{
				OutInfo = (*DropItemInfoRow);
				bIsFind = true;
			}
			i++;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDescent_GameInstance::GetDropInfoByName - DropItemInfoTable -NULL"));
	}

	return bIsFind;
}

bool UDescent_GameInstance::GetDropItemInfoByName(FName NameItem, FDropItem& OutInfo)
{
	bool bIsFind = false;
	FDropItem* DropItemInfoRow;

	if (DropItemInfoTable)
	{
		DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(NameItem, "", false);
		if(DropItemInfoRow)
		{
			OutInfo = *DropItemInfoRow;
			bIsFind = true;
		
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UDescent_GameInstance::GetDropInfoByName - DropItemInfoTable -NULL"));
	}

	return bIsFind;
}
