#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "Inv_FastArray.generated.h"

struct FGameplayTag;
struct FInv_InventoryFastArray;
class UInv_InventoryComponent;
class UInv_InventoryItem;
class UInv_ItemComponent;


/** A single entry in an inventory */
USTRUCT(BlueprintType)
struct FInv_InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
public:
	
	FInv_InventoryEntry() = default;

private:

	friend FInv_InventoryFastArray;
	friend UInv_InventoryComponent;
	
	UPROPERTY()
	TObjectPtr<UInv_InventoryItem> Item = nullptr;
};

/** A list of inventory items */
USTRUCT(BlueprintType)
struct FInv_InventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()
public:
	FInv_InventoryFastArray() : OwnerComponent(nullptr) {}
	FInv_InventoryFastArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}

	TArray<UInv_InventoryItem*> GetAllItems() const;

	// FFastArraySerializer contract
		void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
		void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
		
	// End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaSerializeInfo);

	UInv_InventoryItem* AddEntry(UInv_ItemComponent* ItemComponent);
	UInv_InventoryItem* AddEntry(UInv_InventoryItem* InventoryItem);
	
	void RemoveEntry(UInv_InventoryComponent* ItemComponent);
	void RemoveEntry(UInv_InventoryItem* InventoryItem);

	UInv_InventoryItem* FindFirstItemByType(const FGameplayTag& ItemType) const;

private:

	friend UInv_InventoryComponent;
	
	// Replicated list of items
	UPROPERTY()
	TArray<FInv_InventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FInv_InventoryFastArray> : public TStructOpsTypeTraitsBase2<FInv_InventoryFastArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};