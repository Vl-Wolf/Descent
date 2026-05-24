// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSGameInstance.h"

bool UTDSGameInstance::GetWeaponInfoByName(FName NameWeapon, FWeaponInfo& OutInfo)
{
	if (!WeaponInfoTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSGameInstance::GetWeaponInfoByName - WeaponTable - NULL"));
		return false;
	}
	
	FWeaponInfo* WeaponInfoRow = WeaponInfoTable->FindRow<FWeaponInfo>(NameWeapon, "", false);
	if (!WeaponInfoRow)
		return false;

	OutInfo = *WeaponInfoRow;
	return true;
}

bool UTDSGameInstance::GetDropItemInfoByWeaponName(FName NameItem, FDropItem& OutInfo)
{
	if (!DropItemInfoTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSGameInstance::GetDropInfoByName - DropItemInfoTable - NULL"));
		return false;
	}

	TArray<FName>RowNames = DropItemInfoTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FDropItem* DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(RowName, "", false);
		if (!DropItemInfoRow)
			continue;
		
		if (DropItemInfoRow->WeaponInfo.NameItem == NameItem)
		{
			OutInfo = *DropItemInfoRow;
			return true;
		}
	}
	
	return false;
}

bool UTDSGameInstance::GetDropItemInfoByName(FName NameItem, FDropItem& OutInfo)
{
	if (!DropItemInfoTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSGameInstance::GetDropInfoByName - DropItemInfoTable - NULL"));
		return false;
	}
	
	FDropItem* DropItemInfoRow = DropItemInfoTable->FindRow<FDropItem>(NameItem, "", false);
	if(!DropItemInfoRow)
		return false;
	
	OutInfo = *DropItemInfoRow;
	
	return true;
}
