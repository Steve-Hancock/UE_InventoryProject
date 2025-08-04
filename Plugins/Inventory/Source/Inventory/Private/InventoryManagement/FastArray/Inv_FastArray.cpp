#include "InventoryManagement/FastArray/Inv_FastArray.h"

#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Inv_InventoryItem.h"
#include "Tests/ToolMenusTestUtilities.h"


TArray<UInv_InventoryItem*> FInv_InventoryFastArray::GetAllItems() const
{
	TArray<UInv_InventoryItem*> Results;
	Results.Reserve(Entries.Num());

	for ( const auto& Entry : Entries )
	{
		if (!IsValid(Entry.Item)) continue;
		Results.Add(Entry.Item);
	}
	return Results;
}

void FInv_InventoryFastArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	UInv_InventoryComponent* IC = Cast<UInv_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return;
	for (int32 Index : RemovedIndices)
	{
		IC->OnItemRemoved.Broadcast(Entries[Index].Item);
	}
}

void FInv_InventoryFastArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	UInv_InventoryComponent* IC = Cast<UInv_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return;
	for (int32 Index : AddedIndices)
	{
		IC->OnItemAdded.Broadcast(Entries[Index].Item);
	}
}

bool FInv_InventoryFastArray::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaSerializeInfo)
{
	return FastArrayDeltaSerialize<FInv_InventoryEntry, FInv_InventoryFastArray>(Entries, DeltaSerializeInfo, *this);
}

UInv_InventoryItem* FInv_InventoryFastArray::AddEntry(UInv_ItemComponent* ItemComponent)
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	UInv_InventoryComponent* IC = Cast<UInv_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return nullptr;

	FInv_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = ItemComponent->GetItemManifest().Manifest(OwningActor);

	IC->AddRepSubObject(NewEntry.Item);
	MarkItemDirty(NewEntry);
	return NewEntry.Item;
}

UInv_InventoryItem* FInv_InventoryFastArray::AddEntry(UInv_InventoryItem* InventoryItem)
{
	check(OwnerComponent);
	AActor* Owner = OwnerComponent->GetOwner();
	check(Owner->HasAuthority());

	FInv_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = InventoryItem;
	MarkItemDirty(NewEntry);
	return InventoryItem;
}

void FInv_InventoryFastArray::RemoveEntry(UInv_InventoryComponent* ItemComponent)
{
	// TODO:  Implament Once Item component is more complete.
}

void FInv_InventoryFastArray::RemoveEntry(UInv_InventoryItem* InventoryItem)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++ EntryIt)
	{
		FInv_InventoryEntry& Entry = *EntryIt;
		if ( Entry.Item == InventoryItem)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

UInv_InventoryItem* FInv_InventoryFastArray::FindFirstItemByType(const FGameplayTag& ItemType) const
{
	auto FoundItem = Entries.FindByPredicate([ItemType](const FInv_InventoryEntry& Entry)
	{
		return IsValid(Entry.Item) && Entry.Item->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
	});

	return FoundItem ? FoundItem->Item : nullptr;
}

