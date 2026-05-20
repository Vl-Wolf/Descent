// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSInventoryComponent.h"
#include "TDS/Interface/TDS_IGameActor.h"
#include "TDS/Game/TDSGameInstance.h"
#include "Net/UnrealNetwork.h"


UTDSInventoryComponent::UTDSInventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


int32 UTDSInventoryComponent::FindFirstAvailableSlotExcluding(int32 ExcludeIndex) const
{
	for (int32 i = 0; i < WeaponSlots.Num(); i++)
	{
		if (i != ExcludeIndex && !WeaponSlots[i].NameItem.IsNone())
		{
			return i;
		}
	}
	return INDEX_NONE;
}

bool UTDSInventoryComponent::SwitchWeaponToIndexByNextPreviousIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward)
{
	bool bIsSuccess = false;
	int8 CorrectIndex = ChangeToIndex;
	if (ChangeToIndex > WeaponSlots.Num() - 1)
		CorrectIndex = 0;
	else
		if (ChangeToIndex < 0)
			CorrectIndex = WeaponSlots.Num() - 1;

	FName NewIdWeapon;
	FAdditionalWeaponInfo NewAdditionalInfo;
	int32 NewCurrentIndex = 0;

	if (WeaponSlots.IsValidIndex(CorrectIndex))
	{
		if (!WeaponSlots[CorrectIndex].NameItem.IsNone())
		{
			if (WeaponSlots[CorrectIndex].AdditionalInfo.Round > 0)
			{

				bIsSuccess = true;
			}
			else
			{
				UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
				if (GI)
				{

					FWeaponInfo WeaponInfo;
					GI->GetWeaponInfoByName(WeaponSlots[CorrectIndex].NameItem, WeaponInfo);

					bool bIsFind = false;
					int8 j = 0;
					while (j < AmmoSlots.Num() && !bIsFind)
					{
						if (AmmoSlots[j].WeaponType == WeaponInfo.WeaponType && AmmoSlots[j].Count > 0)
						{
							bIsSuccess = true;
							bIsFind = true;
						}
						j++;
					}
				}
			}
			if (bIsSuccess)
			{
				NewCurrentIndex = CorrectIndex;
				NewIdWeapon = WeaponSlots[CorrectIndex].NameItem;
				NewAdditionalInfo = WeaponSlots[CorrectIndex].AdditionalInfo;
			}
		}
	}
	if (!bIsSuccess)
	{
		int8 Iteration = 0;
		int8 SecondIteration = 0;
		int8 tmpIndex = 0;
		while (Iteration < WeaponSlots.Num() && !bIsSuccess)
		{
			Iteration++;

			if (bIsForward)
			{
				//Seconditeration = 0;

				tmpIndex = ChangeToIndex + Iteration;
			}
			else
			{
				SecondIteration = WeaponSlots.Num() - 1;

				tmpIndex = ChangeToIndex - Iteration;
			}

			if (WeaponSlots.IsValidIndex(tmpIndex))
			{
				if (!WeaponSlots[tmpIndex].NameItem.IsNone())
				{
					if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
					{
						//WeaponGood
						bIsSuccess = true;
						NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
						NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
						NewCurrentIndex = tmpIndex;
					}
					else
					{
						FWeaponInfo WeaponInfo;
						UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

						GI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, WeaponInfo);

						bool bIsFind = false;
						int8 j = 0;
						while (j < AmmoSlots.Num() && !bIsFind)
						{
							if (AmmoSlots[j].WeaponType == WeaponInfo.WeaponType && AmmoSlots[j].Count > 0)
							{
								//WeaponGood
								bIsSuccess = true;
								NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
								NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
								NewCurrentIndex = tmpIndex;
								bIsFind = true;
							}
							j++;
						}
					}
				}
			}
			else
			{
				//go to end of LEFT of array weapon slots
				if (OldIndex != SecondIteration)
				{
					if (WeaponSlots.IsValidIndex(SecondIteration))
					{
						if (!WeaponSlots[SecondIteration].NameItem.IsNone())
						{
							if (WeaponSlots[SecondIteration].AdditionalInfo.Round > 0)
							{
								//WeaponGood
								bIsSuccess = true;
								NewIdWeapon = WeaponSlots[SecondIteration].NameItem;
								NewAdditionalInfo = WeaponSlots[SecondIteration].AdditionalInfo;
								NewCurrentIndex = SecondIteration;
							}
							else
							{
								FWeaponInfo Info;
								UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

								GI->GetWeaponInfoByName(WeaponSlots[SecondIteration].NameItem, Info);

								bool bIsFind = false;
								int8 j = 0;
								while (j < AmmoSlots.Num() && !bIsFind)
								{
									if (AmmoSlots[j].WeaponType == Info.WeaponType && AmmoSlots[j].Count > 0)
									{
										//WeaponGood
										bIsSuccess = true;
										NewIdWeapon = WeaponSlots[SecondIteration].NameItem;
										NewAdditionalInfo = WeaponSlots[SecondIteration].AdditionalInfo;
										NewCurrentIndex = SecondIteration;
										bIsFind = true;
									}
									j++;
								}
							}
						}
					}
				}
				else
				{
					//go to same weapon when start
					if (WeaponSlots.IsValidIndex(SecondIteration))
					{
						if (!WeaponSlots[SecondIteration].NameItem.IsNone())
						{
							if (WeaponSlots[SecondIteration].AdditionalInfo.Round > 0)
							{
								//WeaponGood, it same weapon do nothing
							}
							else
							{
								FWeaponInfo WeaponInfo;
								UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

								GI->GetWeaponInfoByName(WeaponSlots[SecondIteration].NameItem, WeaponInfo);

								bool bIsFind = false;
								int8 j = 0;
								while (j < AmmoSlots.Num() && !bIsFind)
								{
									if (AmmoSlots[j].WeaponType == WeaponInfo.WeaponType)
									{
										if (AmmoSlots[j].Count > 0)
										{
											//WeaponGood, it same weapon do nothing
										}
										else
										{
											//Not find weapon with ammo need init Pistol with infinity ammo
											UE_LOG(LogTemp, Error, TEXT("UTDSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
										}
									}
									j++;
								}
							}
						}
					}
				}
				if (bIsForward)
				{
					SecondIteration++;
				}
				else
				{
					SecondIteration--;
				}

			}
		}
	}
	if (bIsSuccess)
	{
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		SwitchWeaponEvent_OnServer(NewIdWeapon, NewAdditionalInfo, NewCurrentIndex);
	}

	return bIsSuccess;
}

bool UTDSInventoryComponent::SwitchWeaponByIndex(int32 IndexWeaponToChange, int32 PreviousIndex, FAdditionalWeaponInfo PreviousWeaponInfo)
{
	bool bIsSuccess = false;

	FName ToSwitchIdWeapon = GetWeaponNameBySlotIndex(IndexWeaponToChange);
	FAdditionalWeaponInfo ToSwitchAdditionalInfo = GetAdditionalInfoWeapon(IndexWeaponToChange);

	if (!ToSwitchIdWeapon.IsNone())
	{
		SetAdditionalInfoWeapon(PreviousIndex, PreviousWeaponInfo);
		SwitchWeaponEvent_OnServer(ToSwitchIdWeapon, ToSwitchAdditionalInfo, IndexWeaponToChange);

		//check ammo slot for event to player		
		EWeaponType ToSwitchWeaponType;
		if (GetWeaponTypeByNameWeapon(ToSwitchIdWeapon, ToSwitchWeaponType))
		{
			int8 AvailableAmmoForWeapon = -1;
			if (CheckAmmoForWeapon(ToSwitchWeaponType, AvailableAmmoForWeapon))
			{

			}
		}
		bIsSuccess = true;
	}
	return bIsSuccess;
}

FAdditionalWeaponInfo UTDSInventoryComponent::GetAdditionalInfoWeapon(int32 IndexWeapon)
{
	if (!WeaponSlots.IsValidIndex(IndexWeapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::GetAdditionalInfoWeapon - Invalid index - %d"), IndexWeapon);
		return FAdditionalWeaponInfo{};
	}

	return WeaponSlots[IndexWeapon].AdditionalInfo;
}

int32 UTDSInventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
	int32 Result = -1;
	int8 i = 0;
	bool bIsFind = false;
	while (i < WeaponSlots.Num() && !bIsFind)
	{
		if (WeaponSlots[i].NameItem == IdWeaponName)
		{
			bIsFind = true;
			Result = i;
		}
		i++;
	}
	return Result;
}

FName UTDSInventoryComponent::GetWeaponNameBySlotIndex(int32 IndexSlot)
{
	FName Result;

	if (WeaponSlots.IsValidIndex(IndexSlot))
	{
		Result = WeaponSlots[IndexSlot].NameItem;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::GetWeaponNameBySlotIndex - Not Correct index Weapon  - %d"), IndexSlot);
	}
	return Result;
}

bool UTDSInventoryComponent::GetWeaponTypeByIndexSlot(int32 IndexSlot, EWeaponType& WeaponType)
{
	bool bIsFind = false;
	FWeaponInfo OutInfo;
	WeaponType = EWeaponType::RifleType;
	UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	if (GI)
	{
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			GI->GetWeaponInfoByName(WeaponSlots[IndexSlot].NameItem, OutInfo);
			WeaponType = OutInfo.WeaponType;
			bIsFind = true;
		}
	}
	return bIsFind;
}

bool UTDSInventoryComponent::GetWeaponTypeByNameWeapon(FName IdWeaponName, EWeaponType& WeaponType)
{
	bool bIsFind = false;
	FWeaponInfo OutInfo;
	WeaponType = EWeaponType::RifleType;
	UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	if (GI)
	{
		GI->GetWeaponInfoByName(IdWeaponName, OutInfo);
		WeaponType = OutInfo.WeaponType;
		bIsFind = true;
	}
	return bIsFind;
}

void UTDSInventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
	if (!WeaponSlots.IsValidIndex(IndexWeapon))
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - Invalid index - %d"), IndexWeapon);
		return;
	}
	
	WeaponSlots[IndexWeapon].AdditionalInfo = NewInfo;
	WeaponAdditionalInfoChangeEvent_Multicast(IndexWeapon, NewInfo);

}

void UTDSInventoryComponent::AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CountChangeAmmo)
{
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			AmmoSlots[i].Count += CountChangeAmmo;
			if (AmmoSlots[i].Count > AmmoSlots[i].MaxCount)
				AmmoSlots[i].Count = AmmoSlots[i].MaxCount;

			AmmoChangeEvent_Multicast(AmmoSlots[i].WeaponType, AmmoSlots[i].Count);

			bIsFind = true;
		}
		i++;
	}
}

bool UTDSInventoryComponent::CheckAmmoForWeapon(EWeaponType TypeWeapon, int8& AvailableAmmoForWeapon)
{
	AvailableAmmoForWeapon = 0;
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			bIsFind = true;
			AvailableAmmoForWeapon = AmmoSlots[i].Count;
			if (AmmoSlots[i].Count > 0)
			{
				return true;
			}
		}
		i++;
	}

	if (AvailableAmmoForWeapon <= 0)
		WeaponAmmoEmptyEvent_Multicast(TypeWeapon);
	else
		WeaponAmmoAvailableEvent_Multicast(TypeWeapon);

	return false;
}

bool UTDSInventoryComponent::CheckCanTakeAmmo(EWeaponType AmmoType)
{
	bool Result = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !Result)
	{
		if (AmmoSlots[i].WeaponType == AmmoType && AmmoSlots[i].Count < AmmoSlots[i].MaxCount)
			Result = true;
		i++;
	}
	return Result;
}

bool UTDSInventoryComponent::CheckCanTakeWeapon(int32& FreeSlot)
{
	bool bIsFreeSlot = false;
	int8 i = 0;
	while (i < WeaponSlots.Num() && !bIsFreeSlot)
	{
		if (WeaponSlots[i].NameItem.IsNone())
		{
			bIsFreeSlot = true;
			FreeSlot = i;
		}
		i++;
	}
	return bIsFreeSlot;
}

bool UTDSInventoryComponent::SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot, int32 CurrentIndexWeaponChar, FDropItem& DropItemInfo)
{
	bool Result = false;

	if (WeaponSlots.IsValidIndex(IndexSlot) && GetDropItemInfoFromInventory(IndexSlot, DropItemInfo))
	{
		WeaponSlots[IndexSlot] = NewWeapon;

		SwitchWeaponToIndexByNextPreviousIndex(CurrentIndexWeaponChar, -1, NewWeapon.AdditionalInfo, true);

		UpdateWeaponSlotsEvent_Multicast(IndexSlot, NewWeapon);

		Result = true;
	}
	return Result;
}

void UTDSInventoryComponent::TryGetWeaponToInventory_OnServer_Implementation(AActor* PickUpActor, FWeaponSlot NewWeapon)
{
	int32 IndexSlot = -1;
	if (CheckCanTakeWeapon(IndexSlot))
	{
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			WeaponSlots[IndexSlot] = NewWeapon;

			UpdateWeaponSlotsEvent_Multicast(IndexSlot, NewWeapon);

			if (PickUpActor)
			{
				PickUpActor->Destroy();
			}
		}
	}
}

void UTDSInventoryComponent::DropWeaponByIndex_OnServer_Implementation(int32 ByIndex)
{
	FWeaponSlot EmptyWeaponSlot;
	FDropItem DropItemInfo;

	bool bIsCanDrop = false;
	int8 i = 0;
	int8 AvailableWeaponNum = 0;
	while (i < WeaponSlots.Num() && !bIsCanDrop)
	{
		if (!WeaponSlots[i].NameItem.IsNone())
		{
			AvailableWeaponNum++;
			if (AvailableWeaponNum > 1)
				bIsCanDrop = true;
		}
		i++;
	}

	if (bIsCanDrop && WeaponSlots.IsValidIndex(ByIndex) && GetDropItemInfoFromInventory(ByIndex, DropItemInfo))
	{
		
		int32 SwitchToIndex = FindFirstAvailableSlotExcluding(ByIndex);
		if (SwitchToIndex != INDEX_NONE)
		{
			SwitchWeaponEvent_OnServer(WeaponSlots[SwitchToIndex].NameItem, WeaponSlots[SwitchToIndex].AdditionalInfo,
				SwitchToIndex);
		}

		WeaponSlots[ByIndex] = EmptyWeaponSlot;
		if (GetOwner()->GetClass()->ImplementsInterface(UTDS_IGameActor::StaticClass()))
		{
			ITDS_IGameActor::Execute_DropWeaponToWorld(GetOwner(), DropItemInfo);
		}

		UpdateWeaponSlotsEvent_Multicast(ByIndex, EmptyWeaponSlot);		
	}
}

bool UTDSInventoryComponent::GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo)
{
	bool Result = false;

	FName DropItemName = GetWeaponNameBySlotIndex(IndexSlot);

	UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	if (GI)
	{
		Result = GI->GetDropItemInfoByWeaponName(DropItemName, DropItemInfo);
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			DropItemInfo.WeaponInfo.AdditionalInfo = WeaponSlots[IndexSlot].AdditionalInfo;
		}
	}

	return Result;
}

TArray<FWeaponSlot> UTDSInventoryComponent::GetWeaponSlots()
{
	return WeaponSlots;
}

TArray<FAmmoSlot> UTDSInventoryComponent::GetAmmoSlots()
{
	return AmmoSlots;
}

void UTDSInventoryComponent::InitInventory_OnServer_Implementation(const TArray<FWeaponSlot>& NewWeaponSlotsInfo, const TArray<FAmmoSlot>& NewAmmoSlotsInfo)
{
	WeaponSlots = NewWeaponSlotsInfo;
	AmmoSlots = NewAmmoSlotsInfo;
	
	MaxSlotsWeapon = WeaponSlots.Num();

	if (WeaponSlots.IsValidIndex(0))
	{
		if (!WeaponSlots[0].NameItem.IsNone())
			SwitchWeaponEvent_OnServer(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo, 0);
	}
}

void UTDSInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTDSInventoryComponent, WeaponSlots);
	DOREPLIFETIME(UTDSInventoryComponent, AmmoSlots);

}

void UTDSInventoryComponent::SwitchWeaponEvent_OnServer_Implementation(FName WeaponName, FAdditionalWeaponInfo AdditionalInfo, int32 IndexSlot)
{
	OnSwitchWeapon.Broadcast(WeaponName, AdditionalInfo, IndexSlot);
}

void UTDSInventoryComponent::WeaponAdditionalInfoChangeEvent_Multicast_Implementation(int32 IndexWeapon, FAdditionalWeaponInfo AdditionalWeaponInfo)
{
	OnWeaponAdditionalInfoChange.Broadcast(IndexWeapon, AdditionalWeaponInfo);
}

void UTDSInventoryComponent::AmmoChangeEvent_Multicast_Implementation(EWeaponType TypeWeapon, int32 Count)
{
	OnAmmoChange.Broadcast(TypeWeapon, Count);
}

void UTDSInventoryComponent::WeaponAmmoEmptyEvent_Multicast_Implementation(EWeaponType TypeWeapon)
{
	OnWeaponAmmoEmpty.Broadcast(TypeWeapon);
}

void UTDSInventoryComponent::WeaponAmmoAvailableEvent_Multicast_Implementation(EWeaponType TypeWeapon)
{
	OnWeaponAmmoAvailable.Broadcast(TypeWeapon);
}

void UTDSInventoryComponent::UpdateWeaponSlotsEvent_Multicast_Implementation(int32 IndexSlotChange, FWeaponSlot NewInfo)
{
	OnUpdateWeaponSlots.Broadcast(IndexSlotChange, NewInfo);
}

void UTDSInventoryComponent::WeaponNotHaveRoundEvent_Multicast_Implementation(int32 IndexSlotWeapon)
{
	OnWeaponNotHaveRound.Broadcast(IndexSlotWeapon);
}

void UTDSInventoryComponent::WeaponHaveRoundEvent_Multicast_Implementation(int32 IndexSlotWeapon)
{
	OnWeaponHaveRound.Broadcast(IndexSlotWeapon);
}
