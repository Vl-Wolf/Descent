// Fill out your copyright notice in the Description page of Project Settings.


#include "TDSInventoryComponent.h"
#include "TDS/Interface/TDS_IGameActor.h"
#include "TDS/Game/TDSGameInstance.h"
#include "Net/UnrealNetwork.h"


UTDSInventoryComponent::UTDSInventoryComponent()
{
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
	const int32 Num = WeaponSlots.Num();
	if (Num == 0) return false;
	
	int32 StartIndex = ChangeToIndex;
	if (StartIndex >= Num) StartIndex = 0;
	else if (StartIndex < 0) StartIndex = Num - 1;
	
	for (int32 Step = 0; Step < Num; Step++)
	{
		int32 CheckIndex = bIsForward
			? (StartIndex + Step) % Num
			: ((StartIndex - Step) % Num + Num) % Num;

		if (CheckIndex == OldIndex)
			continue;

		if (WeaponSlots.IsValidIndex(CheckIndex) && !WeaponSlots[CheckIndex].NameItem.IsNone())
		{
			SetAdditionalInfoWeapon(OldIndex, OldInfo);
			SwitchWeaponEvent_OnServer(
				WeaponSlots[CheckIndex].NameItem,
				WeaponSlots[CheckIndex].AdditionalInfo,
				CheckIndex);
			return true;
		}
	}

	return false;
}

bool UTDSInventoryComponent::SwitchWeaponByIndex(int32 IndexWeaponToChange, int32 PreviousIndex, FAdditionalWeaponInfo PreviousWeaponInfo)
{
	if (!WeaponSlots.IsValidIndex(IndexWeaponToChange))
		return false;

	const FName ToSwitchName = WeaponSlots[IndexWeaponToChange].NameItem;
	if (ToSwitchName.IsNone())
		return false;

	SetAdditionalInfoWeapon(PreviousIndex, PreviousWeaponInfo);
	SwitchWeaponEvent_OnServer(
		ToSwitchName,
		WeaponSlots[IndexWeaponToChange].AdditionalInfo,
		IndexWeaponToChange);

	return true;
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
	for (int32 i = 0; i < WeaponSlots.Num(); i++)
	{
		if (WeaponSlots[i].NameItem == IdWeaponName)
			return i;
	}
	
	return INDEX_NONE;
}

FName UTDSInventoryComponent::GetWeaponNameBySlotIndex(int32 IndexSlot)
{

	if (!WeaponSlots.IsValidIndex(IndexSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("UTDSInventoryComponent::GetWeaponNameBySlotIndex - Not Correct index Weapon  - %d"), IndexSlot);
		return FName{};
	}

	return WeaponSlots[IndexSlot].NameItem;
}

bool UTDSInventoryComponent::GetWeaponTypeByIndexSlot(int32 IndexSlot, EWeaponType& WeaponType)
{
	UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	if (!GI)
		return false;
	
	if (!WeaponSlots.IsValidIndex(IndexSlot))
		return false;
	
	FWeaponInfo OutInfo;
	GI->GetWeaponInfoByName(WeaponSlots[IndexSlot].NameItem, OutInfo);
	WeaponType = OutInfo.WeaponType;
	
	return true;
}

bool UTDSInventoryComponent::GetWeaponTypeByNameWeapon(FName IdWeaponName, EWeaponType& WeaponType)
{
	UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	if (!GI)
		return false;
	
	FWeaponInfo OutInfo;
	GI->GetWeaponInfoByName(IdWeaponName, OutInfo);
	WeaponType = OutInfo.WeaponType;
	
	return true;
}

int32 UTDSInventoryComponent::CountAvailableWeapons() const
{
	int32 Count = 0;
	for (const FWeaponSlot& Slot : WeaponSlots)
	{
		if (!Slot.NameItem.IsNone())
			Count++;
	}
	return Count;
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
	for (int32 i = 0; i < AmmoSlots.Num(); i++)
	{
		if (AmmoSlots[i].WeaponType != TypeWeapon)
			continue;
		
		AmmoSlots[i].Count = FMath::Clamp(AmmoSlots[i].Count + CountChangeAmmo, 0, AmmoSlots[i].MaxCount);
		
		AmmoChangeEvent_Multicast(AmmoSlots[i].WeaponType, AmmoSlots[i].Count);
		
		break;
	}
}

bool UTDSInventoryComponent::CheckAmmoForWeapon(EWeaponType TypeWeapon, int8& AvailableAmmoForWeapon)
{
	AvailableAmmoForWeapon = 0;
		
	for (const FAmmoSlot& Slot : AmmoSlots)
	{
		if (Slot.WeaponType != TypeWeapon)
			continue;
		
		AvailableAmmoForWeapon = Slot.Count;
		
		if (Slot.Count > 0)
			return true;
		
		break;
	}
	
	WeaponAmmoEmptyEvent_Multicast(TypeWeapon);

	return false;
}

bool UTDSInventoryComponent::CheckCanTakeAmmo(EWeaponType AmmoType)
{
	bool bHasWeapon = false;
	UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());

	for (const FWeaponSlot& Slot : WeaponSlots)
	{
		if (Slot.NameItem.IsNone()) continue;

		FWeaponInfo Info;
		if (GI && GI->GetWeaponInfoByName(Slot.NameItem, Info))
		{
			if (Info.WeaponType == AmmoType)
			{
				bHasWeapon = true;
				break;
			}
		}
	}

	if (!bHasWeapon) return false; 
	
	for (int32 i = 0; i < AmmoSlots.Num(); i++)
	{
		if (AmmoSlots[i].WeaponType == AmmoType && AmmoSlots[i].Count < AmmoSlots[i].MaxCount)
			return true;
	}
	
	return false;
}

bool UTDSInventoryComponent::CheckCanTakeWeapon(int32& FreeSlot)
{	
	for (int32 i = 0; i < WeaponSlots.Num(); i++)
	{
		if (WeaponSlots[i].NameItem.IsNone())
		{
			FreeSlot = i;
			return true;
		}
	}
	
	FreeSlot = INDEX_NONE;
	return false;
}

bool UTDSInventoryComponent::SwitchWeaponToInventory(FWeaponSlot NewWeapon, int32 IndexSlot, int32 CurrentIndexWeaponChar, FDropItem& DropItemInfo)
{
	if (!WeaponSlots.IsValidIndex(IndexSlot) || !GetDropItemInfoFromInventory(IndexSlot, DropItemInfo))
		return false;
	
	WeaponSlots[IndexSlot] = NewWeapon;

	SwitchWeaponToIndexByNextPreviousIndex(CurrentIndexWeaponChar, -1, NewWeapon.AdditionalInfo, true);

	UpdateWeaponSlotsEvent_Multicast(IndexSlot, NewWeapon);
	
	return true;
}

void UTDSInventoryComponent::TryGetWeaponToInventory_OnServer_Implementation(AActor* PickUpActor, FWeaponSlot NewWeapon)
{
	int32 IndexSlot = -1;
	if (!CheckCanTakeWeapon(IndexSlot))
		return;
	
	if (!WeaponSlots.IsValidIndex(IndexSlot))
		return;
	
	WeaponSlots[IndexSlot] = NewWeapon;

	UpdateWeaponSlotsEvent_Multicast(IndexSlot, NewWeapon);

	if (PickUpActor)
	{
		PickUpActor->Destroy();
	}
}

void UTDSInventoryComponent::DropWeaponByIndex_OnServer_Implementation(int32 ByIndex)
{
	if (CountAvailableWeapons() <= 1)
		return;
	
	if (!WeaponSlots.IsValidIndex(ByIndex))
		return;
	
	FDropItem DropItemInfo;
	if (!GetDropItemInfoFromInventory(ByIndex, DropItemInfo))
		return;
	
	int32 SwitchToIndex = FindFirstAvailableSlotExcluding(ByIndex);
	if (SwitchToIndex != INDEX_NONE)
	{
		SwitchWeaponEvent_OnServer(WeaponSlots[SwitchToIndex].NameItem, WeaponSlots[SwitchToIndex].AdditionalInfo,
			SwitchToIndex);
	}

	WeaponSlots[ByIndex] = FWeaponSlot{};
	UpdateWeaponSlotsEvent_Multicast(ByIndex, FWeaponSlot{});	
	
	if (GetOwner()->GetClass()->ImplementsInterface(UTDS_IGameActor::StaticClass()))
	{
		ITDS_IGameActor::Execute_DropWeaponToWorld(GetOwner(), DropItemInfo);
	}
	
}

bool UTDSInventoryComponent::GetDropItemInfoFromInventory(int32 IndexSlot, FDropItem& DropItemInfo)
{
	bool Result = false;

	FName DropItemName = GetWeaponNameBySlotIndex(IndexSlot);

	UTDSGameInstance* GI = Cast<UTDSGameInstance>(GetWorld()->GetGameInstance());
	if (!GI)
		return Result;
	
	Result = GI->GetDropItemInfoByWeaponName(DropItemName, DropItemInfo);
	if (WeaponSlots.IsValidIndex(IndexSlot))
	{
		DropItemInfo.WeaponInfo.AdditionalInfo = WeaponSlots[IndexSlot].AdditionalInfo;
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
