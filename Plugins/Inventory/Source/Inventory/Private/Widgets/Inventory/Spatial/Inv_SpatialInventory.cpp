// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Widgets/Inventory/Spatial/Inv_SpatialInventory.h"

#include "Inventory.h"
#include "Components/Button.h"
#include "Logging/LogMacros.h"
#include "Widgets/Inventory/GridSlots/Inv_EquippedGridSlot.h"
#include "Blueprint/WidgetTree.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Items/Inv_InventoryItem.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/Inv_EquippedSlottedItem.h"
#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"
#include "Widgets/ItemDescription/Inv_ItemDescription.h"

void UInv_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Button_Equippables->OnClicked.AddDynamic(this, &ThisClass::UInv_SpatialInventory::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this, &ThisClass::UInv_SpatialInventory::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &ThisClass::UInv_SpatialInventory::ShowCraftables);
	Button_Collectables->OnClicked.AddDynamic(this, &ThisClass::UInv_SpatialInventory::ShowCollectables);

	Grid_Equippables->SetOwningCanvasPanel(CanvasPanel );
	Grid_Consumables->SetOwningCanvasPanel(CanvasPanel);
	Grid_Craftables->SetOwningCanvasPanel(CanvasPanel);
	Grid_Collectables->SetOwningCanvasPanel(CanvasPanel);

	ShowEquippables();

	WidgetTree->ForEachWidget([this](UWidget* Widget)
	{
		UInv_EquippedGridSlot* EquippedGridSlot = Cast<UInv_EquippedGridSlot>(Widget);
		if (IsValid(EquippedGridSlot))
		{
			EquippedGridSlots.Add(EquippedGridSlot);
			EquippedGridSlot->EquippedGridSlotClicked.AddDynamic(this, &ThisClass::EquippedGridSlotClicked);
		}
	});
}

FReply UInv_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	ActiveGrid->DropItem();
	return FReply::Handled();
}

void UInv_SpatialInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(ItemDescription)) return;
	SetItemDescriptionSizeAndPosition(ItemDescription, CanvasPanel);
}

FInv_SlotAvailabilityResult UInv_SpatialInventory::HasRoomForItem(UInv_ItemComponent* ItemComponent) const
{
	switch ( UInv_InventoryStatics::GetItemCategoryFromItemComponent(ItemComponent))
	{
		case EInv_ItemCategory::Equippable:
			return Grid_Equippables->HasRoomForItem(ItemComponent);
		case EInv_ItemCategory::Consumable:
			return Grid_Consumables->HasRoomForItem(ItemComponent);
		case EInv_ItemCategory::Craftable:
			return Grid_Craftables->HasRoomForItem(ItemComponent);
		case EInv_ItemCategory::Collectable:
			return Grid_Collectables->HasRoomForItem(ItemComponent);
		default:
			UE_LOG(LogInventory, Error, TEXT("Invalid Item Category"));
			return FInv_SlotAvailabilityResult();
	}
}

void UInv_SpatialInventory::OnItemHovered(UInv_InventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	
	UInv_ItemDescription* ItemDescriptionWidget = GetItemDescription();
	ItemDescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);

	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimerHandle);

	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this, &Manifest, ItemDescriptionWidget]()
	{
		Manifest.AssimilateInventoryFragments(ItemDescriptionWidget);
		GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);
	});
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(DescriptionTimerHandle, DescriptionTimerDelegate, DescriptionTimerDuration, false);
}
 
void UInv_SpatialInventory::OnItemUnHovered()
{
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimerHandle);
}

bool UInv_SpatialInventory::HasHoverItem() const
{
	if ( Grid_Collectables->HasHoverItem()) return true;
	if ( Grid_Craftables->HasHoverItem()) return true;
	if ( Grid_Consumables->HasHoverItem()) return true;
	if ( Grid_Equippables->HasHoverItem()) return true;
	return false;
}

UInv_HoverItem* UInv_SpatialInventory::GetHoverItem() const
{
	if (!ActiveGrid.IsValid()) return nullptr;

	return ActiveGrid->GetHoverItem();
}

float UInv_SpatialInventory::GetTileSize() const
{
	return Grid_Equippables->GetTileSize();
}

UInv_ItemDescription* UInv_SpatialInventory::GetItemDescription()
{
	if ( !IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UInv_ItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	return ItemDescription;
}

void UInv_SpatialInventory::ShowEquippables()
{
	SetActiveGrid(Grid_Equippables, Button_Equippables);
}

void UInv_SpatialInventory::ShowConsumables()
{
	SetActiveGrid(Grid_Consumables, Button_Consumables);
}

void UInv_SpatialInventory::ShowCraftables()
{
	SetActiveGrid(Grid_Craftables, Button_Craftables);
}

void UInv_SpatialInventory::ShowCollectables()
{
	SetActiveGrid(Grid_Collectables, Button_Collectables);
}

void UInv_SpatialInventory::EquippedGridSlotClicked(UInv_EquippedGridSlot* EquippedGridSlot,
	const FGameplayTag& EquipmentTypeTag)
{
	// Check to see if we can equip the Hover Item
	if (!CanEquipHoverItem(EquippedGridSlot, EquipmentTypeTag)) return;

	// Get the Hover Item
	UInv_HoverItem* HoverItem = GetHoverItem();

	
	// Create an Equipped Slotted item and add it to the Equipped Grid Slot (call EquippedGridSlot->OnItemEquipped())
	const float TileSize = UInv_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize();
	UInv_EquippedSlottedItem* EquippedSlottedItem = EquippedGridSlot->OnItemEquipped(
		HoverItem->GetInventoryItem(),
		EquipmentTypeTag,
		TileSize
	);

	EquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	
	// Clear Hover Item
	Grid_Equippables->ClearHoverItem();
	// Inform the server that we equipped an Item (Potentially unequipping an item as well)
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));

	InventoryComponent->Server_EquipSlotClicked(HoverItem->GetInventoryItem(), nullptr);

	if (GetOwningPlayer()->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		InventoryComponent->OnItemEquipped.Broadcast(HoverItem->GetInventoryItem());
	}
}

void UInv_SpatialInventory::EquippedSlottedItemClicked(UInv_EquippedSlottedItem* EquippedSlottedItem)
{
	
	// Remove the Item Description
	UInv_InventoryStatics::ItemUnHovered(GetOwningPlayer());

	UInv_HoverItem* HoverItem = GetHoverItem();
	if (IsValid(HoverItem) && HoverItem->GetIsStackable()) return;

	// Get Item to Equip
	UInv_InventoryItem* ItemToEquip = IsValid(HoverItem) ? HoverItem->GetInventoryItem() : nullptr;

	// Get Item to Unequip
	UInv_InventoryItem* ItemToUnequip = EquippedSlottedItem->GetInventoryItem();

	// Get the Equipped Grid slot Holding this item
	UInv_EquippedGridSlot* EquippedGridSlot = FindSlotWithEquippedItem(ItemToUnequip);
	
	// Clear the equipped grid slot of this item (set its inventory item to null)
	ClearSlotOfItem(EquippedGridSlot);

	// Assign a previously equipped item as the hover item.
	Grid_Equippables->AssignHoverItem(ItemToUnequip);
	
	// Remove the equipped slotted item from the equipped grid slot
	RemovedEquippedSlottedItem(EquippedSlottedItem);
	
	// Make a new equipped slotted item (for the item we held in HoverItem)
	MakeEquippedSlottedItem(EquippedSlottedItem, EquippedGridSlot, ItemToEquip);
	
	// Broadcast delegates for OnItemEquipped / OnItemUnequipped (From the Inventory Component)
	BroadcastSlotClickedDelegates(ItemToEquip, ItemToUnequip);
}

void UInv_SpatialInventory::DisableButton(UButton* Button)
{
	Button_Equippables->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	Button_Collectables->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}

void UInv_SpatialInventory::SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Button)
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->HideCursor();
		ActiveGrid->OnHide();
	}
	
	ActiveGrid = Grid;
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->ShowCursor();
	}
	DisableButton(Button);
	Switcher->SetActiveWidget(ActiveGrid.Get());
}

void UInv_SpatialInventory::SetItemDescriptionSizeAndPosition(UInv_ItemDescription* Description,
	UCanvasPanel* Canvas) const
{
	UCanvasPanelSlot* ItemDescriptionCanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Description);
	if (!IsValid(ItemDescriptionCanvasSlot)) return;

	const FVector2D ItemDescriptionSize = Description->GetBoxSize();
	ItemDescriptionCanvasSlot->SetSize(ItemDescriptionSize);

	FVector2D ClampedPosition = UInv_WidgetUtils::GetClampedWidgetPosition(
		UInv_WidgetUtils::GetWidgetSize(Canvas),
		ItemDescriptionSize,
		UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer()));

	ItemDescriptionCanvasSlot->SetPosition(ClampedPosition);
}

bool UInv_SpatialInventory::CanEquipHoverItem(UInv_EquippedGridSlot* EquippedGridSlot,
	const FGameplayTag& EquipmentTypeTag) const
{
	if (!IsValid(EquippedGridSlot)) return false;
	if (EquippedGridSlot->GetInventoryItem().IsValid()) return false;

	UInv_HoverItem* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem)) return false;

	UInv_InventoryItem* HeldItem = HoverItem->GetInventoryItem();
	return HasHoverItem() &&
		IsValid(HeldItem) &&
		HeldItem->GetItemManifest().GetItemCategory() == EInv_ItemCategory::Equippable &&
		HeldItem->GetItemManifest().GetItemType().MatchesTag(EquipmentTypeTag);
}

UInv_EquippedGridSlot* UInv_SpatialInventory::FindSlotWithEquippedItem(const UInv_InventoryItem* EquippedItem) const
{
	auto* FoundEquippedGridSlot = EquippedGridSlots.FindByPredicate([=](UInv_EquippedGridSlot* GridSlot)
	{
		return GridSlot->GetInventoryItem() == EquippedItem;
	});
	return FoundEquippedGridSlot ? *FoundEquippedGridSlot : nullptr;
}

void UInv_SpatialInventory::ClearSlotOfItem(UInv_EquippedGridSlot* EquippedGridSlot)
{
	if (IsValid(EquippedGridSlot))
	{
		EquippedGridSlot->SetEquippedSlottedItem(nullptr);
		EquippedGridSlot->SetInventoryItem(nullptr);
	}
}

void UInv_SpatialInventory::RemovedEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem)
{
	if (!IsValid(EquippedSlottedItem)) return;

	if (EquippedSlottedItem->OnEquippedSlottedItemClicked.IsAlreadyBound(this, &ThisClass::EquippedSlottedItemClicked))
	{
		EquippedSlottedItem->OnEquippedSlottedItemClicked.RemoveDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	}
	EquippedSlottedItem->RemoveFromParent();
}

void UInv_SpatialInventory::MakeEquippedSlottedItem(UInv_EquippedSlottedItem* EquippedSlottedItem,
	UInv_EquippedGridSlot* EquippedGridSlot, UInv_InventoryItem* ItemToEquip)
{
	if (!IsValid(EquippedGridSlot)) return;

	UInv_EquippedSlottedItem* NewSlottedItem = EquippedGridSlot->OnItemEquipped(ItemToEquip,
		EquippedSlottedItem->GetEquipmentTypeTag(),
		UInv_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize());

	if (IsValid(NewSlottedItem))
	{
		NewSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	}
	
	EquippedGridSlot->SetEquippedSlottedItem(NewSlottedItem);
}

void UInv_SpatialInventory::BroadcastSlotClickedDelegates(UInv_InventoryItem* ItemToEquip,
	UInv_InventoryItem* ItemToUnequip) const
{
	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));
	InventoryComponent->Server_EquipSlotClicked(ItemToEquip, ItemToUnequip);

	if ( GetOwningPlayer()->GetNetMode() != ENetMode::NM_DedicatedServer)
	{
		InventoryComponent->OnItemEquipped.Broadcast(ItemToEquip);
		InventoryComponent->OnItemUnequipped.Broadcast(ItemToUnequip);
	}
}
