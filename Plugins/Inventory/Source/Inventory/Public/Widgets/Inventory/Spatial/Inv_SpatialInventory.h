// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"
#include "Inv_SpatialInventory.generated.h"

class UInv_EquippedSlottedItem;
struct FGameplayTag;
class UInv_EquippedGridSlot;
class UInv_ItemDescription;
class UCanvasPanel;
class UInv_InventoryGrid;
class UWidgetSwitcher;
class UButton;

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_SpatialInventory : public UInv_InventoryBase
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	virtual FInv_SlotAvailabilityResult HasRoomForItem(UInv_ItemComponent* ItemComponent) const override;
	virtual void OnItemHovered(UInv_InventoryItem* Item) override;
	virtual void OnItemUnHovered() override;
	virtual bool HasHoverItem() const override;
	virtual UInv_HoverItem* GetHoverItem() const override;
	virtual float GetTileSize() const override;
	
private:

	TArray<TObjectPtr<UInv_EquippedGridSlot>> EquippedGridSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Equippables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Consumables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Craftables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInv_InventoryGrid> Grid_Collectables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Equippables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Consumables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Craftables;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Collectables;

	UInv_ItemDescription* GetItemDescription();
	

	UFUNCTION()
	void ShowEquippables();

	UFUNCTION()
	void ShowConsumables();

	UFUNCTION()
	void ShowCraftables();

	UFUNCTION()
	void ShowCollectables();

	UFUNCTION()
	void EquippedGridSlotClicked(UInv_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag);

	UFUNCTION()
	void EquippedSlottedItemClicked(UInv_EquippedSlottedItem* EquippedSlottedItem);
	
	void DisableButton(UButton* Button);
	void SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Button);
	void SetItemDescriptionSizeAndPosition(UInv_ItemDescription* Description, UCanvasPanel* Canvas) const;
	bool CanEquipHoverItem(UInv_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const;
	UInv_EquippedGridSlot* FindSlotWithEquippedItem(const UInv_InventoryItem* EquippedItem) const;
	void ClearSlotOfItem(UInv_EquippedGridSlot* EquippedGridSlot);
	void RemovedEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem);
	void MakeEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem, UInv_EquippedGridSlot* EquippedGridSlot, UInv_InventoryItem* ItemToEquip );
	void BroadcastSlotClickedDelegates(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnequip) const;

	
	TWeakObjectPtr<UInv_InventoryGrid> ActiveGrid;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_ItemDescription> ItemDescriptionClass;

	UPROPERTY()
	TObjectPtr<UInv_ItemDescription> ItemDescription;

	FTimerHandle DescriptionTimerHandle;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DescriptionTimerDuration{0.5f};
};
