// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSInventoryComponent.h"
#include "TDS/Game/TDSGameInstance.h"

// Sets default values for this component's properties
UTDSInventoryComponent::UTDSInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTDSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	for (int8 i = 0; i < WeaponSlots.Num(); i++)
	{
		UTDSGameInstance* MyGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
		if (MyGI)
		{
			if (!WeaponSlots[i].NameItem.IsNone())
			{
				FWeaponInfo Info;
				if (MyGI->GetWeaponInfoByName(WeaponSlots[i].NameItem, Info))
				{
					WeaponSlots[i].AdditionalInfo.Round = Info.MaxRound;
				}
				else
				{

				}
			}
		}
	}

	MaxSlotsWeapon = WeaponSlots.Num();

	if (WeaponSlots.IsValidIndex(0))
	{
		if (!WeaponSlots[0].NameItem.IsNone())
		{
			OnSwitchWeapon.Broadcast(WeaponSlots[0].NameItem, WeaponSlots[0].AdditionalInfo, 0);
		}
	}
}


// Called every frame
void UTDSInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UTDSInventoryComponent::SwitchWeaponToIndex(int32 ChangeToIndex, int32 OldIndex, FAdditionalWeaponInfo OldInfo, bool bIsForward)
{
	bool bIsSuccess;
	int8 CorrectIndex = ChangeToIndex;

	if (ChangeToIndex > WeaponSlots.Num() - 1)
	{
		CorrectIndex = 0;
	}
	else
	{
		if (ChangeToIndex < 0)
		{
			CorrectIndex = WeaponSlots.Num() - 1;
		}
	}
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
				UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
				if (myGI)
				{
					FWeaponInfo myInfo;
					myGI->GetWeaponInfoByName(WeaponSlots[CorrectIndex].NameItem, myInfo);
					bool bIsFind = false;
					int8 j = 0;
					while (j < AmmoSlots.Num() && !bIsFind)
					{
						if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
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
		if (bIsForward)
		{
			int8 iteration = 0;
			int8 Seconditeration = 0;
			while (iteration < WeaponSlots.Num() && !bIsSuccess)
			{
				iteration++;
				int8 tmpIndex = ChangeToIndex + iteration;
				if (WeaponSlots.IsValidIndex(tmpIndex))
				{
					if (!WeaponSlots[tmpIndex].NameItem.IsNone())
					{
						if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
						{
							bIsSuccess = true;
							NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
							NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
							NewCurrentIndex = tmpIndex;
						}
						else
						{
							FWeaponInfo myInfo;
							UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
							myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

							bool bIsFind = false;
							int8 j = 0;
							while (j < AmmoSlots.Num() && !bIsFind)
							{
								if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
								{
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
					if (OldIndex != Seconditeration)
					{
						if (WeaponSlots.IsValidIndex(Seconditeration))
						{
							if (!WeaponSlots[Seconditeration].NameItem.IsNone())
							{
								if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
								{
									bIsSuccess = true;
									NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
									NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
									NewCurrentIndex = Seconditeration;
								}
								else
								{
									FWeaponInfo myInfo;
									UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
									myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
										{
											bIsSuccess = true;
											NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
											NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
											NewCurrentIndex = Seconditeration;
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
						if (WeaponSlots.IsValidIndex(Seconditeration))
						{
							if (!WeaponSlots[Seconditeration].NameItem.IsNone())
							{
								if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
								{
									
								}
								else
								{
									FWeaponInfo myInfo;
									UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
									myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
										{
											if (AmmoSlots[j].Cout > 0)
											{

											}
											else
											{
												UE_LOG(LogTemp, Error, TEXT("UTDSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
											}
										}
										j++;
									}
								}
							}
						}
					}
					Seconditeration++;
				}
			}
		}
		else
		{
			int8 iteration = 0;
			int8 Seconditeration = WeaponSlots.Num() - 1;
			while (iteration < WeaponSlots.Num() && !bIsSuccess)
			{
				iteration++;
				int8 tmpIndex = ChangeToIndex - iteration;
				if (WeaponSlots.IsValidIndex(tmpIndex))
				{
					if (!WeaponSlots[tmpIndex].NameItem.IsNone())
					{
						if (WeaponSlots[tmpIndex].AdditionalInfo.Round > 0)
						{
							bIsSuccess = true;
							NewIdWeapon = WeaponSlots[tmpIndex].NameItem;
							NewAdditionalInfo = WeaponSlots[tmpIndex].AdditionalInfo;
							NewCurrentIndex = tmpIndex;
						}
						else
						{
							FWeaponInfo myInfo;
							UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
							myGI->GetWeaponInfoByName(WeaponSlots[tmpIndex].NameItem, myInfo);

							bool bIsFind = false;
							int8 j = 0;
							while (j < AmmoSlots.Num() && !bIsFind)
							{
								if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
								{
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
					if (OldIndex != Seconditeration)
					{
						if (WeaponSlots.IsValidIndex(Seconditeration))
						{
							if (!WeaponSlots[Seconditeration].NameItem.IsNone())
							{
								if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
								{
									bIsSuccess = true;
									NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
									NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
									NewCurrentIndex = Seconditeration;
								}
								else
								{
									FWeaponInfo myInfo;
									UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
									myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType && AmmoSlots[j].Cout > 0)
										{
											bIsSuccess = true;
											NewIdWeapon = WeaponSlots[Seconditeration].NameItem;
											NewAdditionalInfo = WeaponSlots[Seconditeration].AdditionalInfo;
											NewCurrentIndex = Seconditeration;
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
						if (WeaponSlots.IsValidIndex(Seconditeration))
						{
							if (!WeaponSlots[Seconditeration].NameItem.IsNone())
							{
								if (WeaponSlots[Seconditeration].AdditionalInfo.Round > 0)
								{

								}
								else
								{
									FWeaponInfo myInfo;
									UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
									myGI->GetWeaponInfoByName(WeaponSlots[Seconditeration].NameItem, myInfo);

									bool bIsFind = false;
									int8 j = 0;
									while (j < AmmoSlots.Num() && !bIsFind)
									{
										if (AmmoSlots[j].WeaponType == myInfo.WeaponType)
										{
											if (AmmoSlots[j].Cout > 0)
											{

											}
											else
											{
												UE_LOG(LogTemp, Error, TEXT("UTDSInventoryComponent::SwitchWeaponToIndex - Init PISTOL - NEED"));
											}
										}
										j++;
									}
								}
							}
						}
					}
					Seconditeration--;
				}
			}
		}
	}

	if (bIsSuccess)
	{
		SetAdditionalInfoWeapon(OldIndex, OldInfo);
		OnSwitchWeapon.Broadcast(NewIdWeapon, NewAdditionalInfo, NewCurrentIndex);
	}

	return bIsSuccess;
}

FAdditionalWeaponInfo UTDSInventoryComponent::GettAdditionalInfoWeapon(int32 IndexWeapon)
{
	FAdditionalWeaponInfo result;
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			if (i == IndexWeapon)
			{
				result = WeaponSlots[i].AdditionalInfo;
				bIsFind = true;
			}
			i++;
		}
		if (!bIsFind)
		{
			UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - No Found Weapon with index - %d"), IndexWeapon);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - No Correct index Weapon - %d"), IndexWeapon);
	}

	return result;
}

int32 UTDSInventoryComponent::GetWeaponIndexSlotByName(FName IdWeaponName)
{
	int32 result = -1;
	int8 i = 0;
	bool bIsFind = false;

	while (i < WeaponSlots.Num() && !bIsFind)
	{
		if (WeaponSlots[i].NameItem == IdWeaponName)
		{
			bIsFind = true;
			result = i;
		}
		i++;
	}

	return result;
}

FName UTDSInventoryComponent::GetWeaponNameBySlotIndex(int32 IndexSlot)
{
	FName result;
	if (WeaponSlots.IsValidIndex(IndexSlot))
	{
		result = WeaponSlots[IndexSlot].NameItem;
	}

	return result;
}

void UTDSInventoryComponent::SetAdditionalInfoWeapon(int32 IndexWeapon, FAdditionalWeaponInfo NewInfo)
{
	if (WeaponSlots.IsValidIndex(IndexWeapon))
	{
		bool bIsFind = false;
		int8 i = 0;
		while (i < WeaponSlots.Num() && !bIsFind)
		{
			if (i == IndexWeapon)
			{
				WeaponSlots[i].AdditionalInfo = NewInfo;
				bIsFind = true;
				OnWeaponAdditionalInfoChange.Broadcast(IndexWeapon, NewInfo);
			}
			i++;
		}
		if (!bIsFind)
		{
			UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - No Found Weapon with index - %d"), IndexWeapon);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::SetAdditionalInfoWeapon - No Correct index Weapon - %d"), IndexWeapon);
	}
}

void UTDSInventoryComponent::AmmoSlotChangeValue(EWeaponType TypeWeapon, int32 CoutChangeAmmo)
{
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			AmmoSlots[i].Cout += CoutChangeAmmo;
			if (AmmoSlots[i].Cout > AmmoSlots[i].MaxCout)
			{
				AmmoSlots[i].Cout = AmmoSlots[i].MaxCout;
			}

			OnAmmoChange.Broadcast(AmmoSlots[i].WeaponType, AmmoSlots[i].Cout);
			bIsFind = true;
		}
		i++;
	}
}

bool UTDSInventoryComponent::CheckAmmoForWeapon(EWeaponType TypeWeapon, int8& AviableAmmoForWeapon)
{
	AviableAmmoForWeapon = 0;
	bool bIsFind = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !bIsFind)
	{
		if (AmmoSlots[i].WeaponType == TypeWeapon)
		{
			bIsFind = true;
			AviableAmmoForWeapon = AmmoSlots[i].Cout;
			if (AmmoSlots[i].Cout > 0)
			{
				return true;
			}
			
		}
		i++;
	}

	OnWeaponAmmoEmpty.Broadcast(TypeWeapon);

	return false;
}

bool UTDSInventoryComponent::CheckCanTakeAmmo(EWeaponType AmmoType)
{
	bool result = false;
	int8 i = 0;
	while (i < AmmoSlots.Num() && !result)
	{
		if (AmmoSlots[i].WeaponType == AmmoType && AmmoSlots[i].Cout < AmmoSlots[i].MaxCout)
		{
			result = true;
		}
		i++;
	}
	return result;
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
	bool result = false;
	if (WeaponSlots.IsValidIndex(IndexSlot) && GetDropItemInfoFromInventory(IndexSlot, DropItemInfo))
	{
		WeaponSlots[IndexSlot] = NewWeapon;
		SwitchWeaponToIndex(CurrentIndexWeaponChar, -1, NewWeapon.AdditionalInfo, true);
		OnUpdateWeaponSlots.Broadcast(IndexSlot, NewWeapon);
		result = true;
	}

	return result;
}

bool UTDSInventoryComponent::TryGetWeaponToInventory(FWeaponSlot NewWeapon)
{
	int32 indexSlot = -1;
	if (CheckCanTakeWeapon(indexSlot))
	{
		if (WeaponSlots.IsValidIndex(indexSlot))
		{
			WeaponSlots[indexSlot] = NewWeapon;
			OnUpdateWeaponSlots.Broadcast(indexSlot, NewWeapon);
			return true;
		}
	}

	return false;
}

bool UTDSInventoryComponent::GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo)
{
	bool result = false;
	FName DropItemName = GetWeaponNameBySlotIndex(IndexSlot);
	UTDSGameInstance* myGI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	if (myGI)
	{
		result = myGI->GetDropItemInfoByWeaponName(DropItemName, DropItemInfo);
		if (WeaponSlots.IsValidIndex(IndexSlot))
		{
			DropItemInfo.WeaponInfo.AdditionalInfo = WeaponSlots[IndexSlot].AdditionalInfo;
		}
	}
	return result;
}

