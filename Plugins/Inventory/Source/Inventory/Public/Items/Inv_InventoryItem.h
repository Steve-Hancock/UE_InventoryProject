// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Items/Manifest/Inv_ItemManifest.h"

#include "Inv_InventoryItem.generated.h"


/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_InventoryItem : public UObject
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }
	bool IsStackable() const;
	bool IsConsumable() const;
	
	void SetItemManifest(const FInv_ItemManifest& Manifest);
	void SetTotalStackCount(int32 Count);
	
	const FInv_ItemManifest& GetItemManifest() const {return ItemManifest.Get<FInv_ItemManifest>();}
	FInv_ItemManifest& GetItemManifestMutable() {return ItemManifest.GetMutable<FInv_ItemManifest>(); }
	int32 GetTotalStackCount() const {return TotalStackCount; }

private:

	UPROPERTY(VisibleAnywhere, META = (BaseStruct = "/Script/Inventory.Inv_ItemManifest"), Replicated, Category = "Inventory")
	FInstancedStruct ItemManifest;

	UPROPERTY(Replicated)
	int32 TotalStackCount{0};
};

template <typename FragmentType>
const FragmentType* GetFragment(const UInv_InventoryItem* Item, const FGameplayTag& FragmentTag)
{
	if (!IsValid(Item)) return nullptr;

	const FInv_ItemManifest& Manifest = Item->GetItemManifest();
	return Manifest.GetFragmentOfTypeWithTag<FragmentType>(FragmentTag);
	
}