// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Widgets/Inventory/GridSlots/Inv_GridSlot.h"
#include "Widgets/Inventory/SlottedItems/Inv_SlottedItem.h"
#include "Widgets/Utils/Inv_WidgetUtils.h"
#include "GameplayTagContainer.h"
#include "Inventory.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Widgets/ItemPopUp/Inv_ItemPopUp.h"

void UInv_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ConstructGrid();

	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &ThisClass::AddItem);
	InventoryComponent->OnStackChange.AddDynamic(this, &ThisClass::AddStacks);
	InventoryComponent->OnInventoryMenuToggled.AddDynamic(this, &ThisClass::UInv_InventoryGrid::OnInventoryMenuToggled);
}

void UInv_InventoryGrid::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2d CanvasPosition =  UInv_WidgetUtils::GetWidgetPosition(CanvasPanel);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	
	if ( CursorExitedCanvas(CanvasPosition,  UInv_WidgetUtils::GetWidgetSize(CanvasPanel), MousePosition) )
	{
		return;
	}
	UpdateTileParameters(CanvasPosition, MousePosition);
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_ItemComponent* ItemComponent)
{
	return HasRoomForItem(ItemComponent->GetItemManifest());
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const UInv_InventoryItem* Item, const int32 StackAmountOverride)
{
	return HasRoomForItem(Item->GetItemManifest(), StackAmountOverride);
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem(const FInv_ItemManifest& ItemManifest, const int32 StackAmountOverride)
{
	FInv_SlotAvailabilityResult Result;

	// Determine if the item is stackable.
	const FInv_StackableFragment* StackableFragment = ItemManifest.GetFragmentOfType<FInv_StackableFragment>();
	Result.bStackable = StackableFragment != nullptr;
	
	// Determine how many stacks to add.
	const int32 MaxStackSize = Result.bStackable ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = Result.bStackable ? StackableFragment->GetStackCount() : 1;
	if (StackAmountOverride != -1 && Result.bStackable)
	{
		AmountToFill = StackAmountOverride;
	}

	TSet<int32> CheckedIndices;
	// For each Grid slot:
	for  (const auto& GridSlot : GridSlots )
	{
		// If we don't have anymore to fill, break out of the loop early.
		if (AmountToFill == 0) break;
		
		// Is this index claimed yet?
		if (IsIndexClaimed(CheckedIndices, GridSlot->GetIndex())) continue;

		// Is Item in Grid bounds?
		if (!IsInGridBounds(GridSlot->GetIndex(), GetItemDimensions(ItemManifest))) continue;
		
		// Can the item fit here based on grid decisions and grid bounds?
		TSet<int32> TentativeClaimed;
		if (!HasRoomAtIndex(GridSlot, GetItemDimensions(ItemManifest), CheckedIndices, TentativeClaimed, ItemManifest.GetItemType(), MaxStackSize))
		{
			continue;
		}
		
		// How much to fill?
		const int32 AmountToFillInSlot = DetermineFillAmountForSlot(Result.bStackable, MaxStackSize, AmountToFill, GridSlot);
		if ( AmountToFillInSlot == 0) continue;
		
		CheckedIndices.Append(TentativeClaimed);
		// Update the amount left to fill
		Result.TotalRoomToFill += AmountToFillInSlot;
		Result.SlotAvailabilities.Emplace(
			FInv_SlotAvailability{
				HasValidItem(GridSlot) ? GridSlot->GetUpperLeftIndex() : GridSlot->GetIndex(),
				Result.bStackable ? AmountToFillInSlot : 0,
				HasValidItem(GridSlot)
			}
		);

		AmountToFill -= AmountToFillInSlot;
		// How much is the Remainder?
		Result.Remainder = AmountToFill;
		if ( AmountToFill == 0) return Result;
		
	}
	
	return Result;
}

bool UInv_InventoryGrid::HasRoomAtIndex(const UInv_GridSlot* GridSlot, const FIntPoint& Dimensions,
	const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const int32 MaxStackSize)
{
	// Is there room at this index, are there other items in the way?
	bool bHasRoomAtIndex = true;

	UInv_InventoryStatics::ForEach2D(GridSlots, GridSlot->GetIndex(), Dimensions, Columns, [&](const UInv_GridSlot* SubGridSlot)
	{
		if ( CheckSlotConstraints (GridSlot, SubGridSlot, CheckedIndices, OutTentativelyClaimed, ItemType, MaxStackSize) )
		{
			OutTentativelyClaimed.Add(SubGridSlot->GetIndex());
		}
		else
		{
			bHasRoomAtIndex = false;
		}
	});
	
	return bHasRoomAtIndex;
}


FIntPoint UInv_InventoryGrid::GetItemDimensions(const FInv_ItemManifest& Manifest) const
{
	const FInv_GridFragment* GridFragment = Manifest.GetFragmentOfType<FInv_GridFragment>();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
	return Dimensions;	
}

bool UInv_InventoryGrid::CheckSlotConstraints(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot,const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, const int32 MaxStackSize) const
{

	if (IsIndexClaimed(CheckedIndices, SubGridSlot->GetIndex())) return false;
	if (!HasValidItem(SubGridSlot))
	{
		OutTentativelyClaimed.Add(SubGridSlot->GetIndex());
		return true;
	}
	if (!IsUpperLeftSlot(GridSlot, SubGridSlot)) return false;
	const UInv_InventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable()) return false;
	if (!DoesItemTypeMatch(SubItem, ItemType) ) return false;
	if (GridSlot->GetStackCount() >= MaxStackSize) return false;
	
	return true;
}

bool UInv_InventoryGrid::HasValidItem(const UInv_GridSlot* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}

bool UInv_InventoryGrid::IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const
{
	return GridSlot->GetIndex() == SubGridSlot->GetUpperLeftIndex();
}

bool UInv_InventoryGrid::DoesItemTypeMatch(const UInv_InventoryItem* SubItem, const FGameplayTag& ItemType) const
{
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

bool UInv_InventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const
{
	if ( StartIndex < 0 || StartIndex >= GridSlots.Num() ) return false;
	const int32 EndColum = ( StartIndex % Columns) + ItemDimensions.X;
	const int32 EndRow = ( StartIndex / Columns) + ItemDimensions.Y;
	return EndColum <= Columns && EndRow <= Rows;
}

int32 UInv_InventoryGrid::DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize,
	const int32 AmountToFill, const UInv_GridSlot* GridSlot) const
{
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot);
	return bStackable ? FMath::Min(RoomInSlot, AmountToFill) : 1;
}

int32 UInv_InventoryGrid::GetStackAmount(const UInv_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackAmount = GridSlot->GetStackCount();
	// If we are at a slot that doesn't hold the stack count, we must get the actual stack count.
	if ( const int32 UpperLeftIndex = GridSlot->GetUpperLeftIndex(); UpperLeftIndex != INDEX_NONE)
	{
		UInv_GridSlot* UpperLeftSlot = GridSlots[UpperLeftIndex];
		CurrentSlotStackAmount = UpperLeftSlot->GetStackCount();
	}

	return CurrentSlotStackAmount;
}

bool UInv_InventoryGrid::IsRightClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

bool UInv_InventoryGrid::IsLeftClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

void UInv_InventoryGrid::AddStacks(const FInv_SlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get())) return;
	for (const auto& Availability : Result.SlotAvailabilities )
	{
		if (Availability.bItemAtIndex)
		{
			const auto& GridSlot = GridSlots[Availability.Index];
			const auto& SlottedItem = SlottedItems.FindChecked(Availability.Index);
			SlottedItem->UpdateStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
			GridSlot->SetStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
		}
		else
		{
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
			UpdateGridSlots(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
		}
	}
}

void UInv_InventoryGrid::OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{

	UInv_InventoryStatics::ItemUnHovered(GetOwningPlayer());
	
	check (GridSlots.IsValidIndex(GridIndex));
	UInv_InventoryItem* ClickedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();

	if (!IsValid(HoverItem) && IsLeftClick(MouseEvent))
	{
		PickUp(ClickedInventoryItem, GridIndex);
		return;
	}

	if ( !IsValid(HoverItem) && IsRightClick(MouseEvent))
	{
		CreateItemPopUp(GridIndex);
		return;
	}

	// Do the hover item and clicked inventory item share a type and are they stackable?
	if ( IsSameStackable(ClickedInventoryItem))
	{
		const int32 ClickedStackCount = GridSlots[GridIndex]->GetStackCount();
		const FInv_StackableFragment* StackableFragment = GetFragment<FInv_StackableFragment>(ClickedInventoryItem, FragmentTags::StackableFragment);
		const int32 MaxStackSize = StackableFragment->GetMaxStackSize();
		const int32 RoomInClickedSlot = MaxStackSize - ClickedStackCount;

		const int32 HoverStackCount = HoverItem->GetStackCount();
		
		
		// Should we swap their stack counts? (Room in clicked slot == 0 && HoverStack count < MaxStackSize)
		if (ShouldSwapStackCounts(RoomInClickedSlot, HoverStackCount, MaxStackSize))
		{
			SwapStackCounts(ClickedStackCount, HoverStackCount, GridIndex);
			return;
		}
		// Should we consume the hover item's stacks? (Room in clicked slot >= HoveredStackCount)
		if (ShouldConsumeHoverItemStacks(HoverStackCount, RoomInClickedSlot))
		{
			ConsumeHoverItemStacks(ClickedStackCount, HoverStackCount, GridIndex);
			return;
		}
		
		
		// Should we fill in the stacks of the clicked item? (Not consume but update the hover item)
		if ( ShouldFillInStack(RoomInClickedSlot, HoverStackCount))
		{
			FillInStack(RoomInClickedSlot, HoverStackCount - RoomInClickedSlot, GridIndex);
			return;
		}
		// Clicked Slot is already full - do nothing (maybe play a sound?)
		if (RoomInClickedSlot == 0) return;
	}
	
	// Make sure we can swqp with a valid item
	if (CurrentQueryResult.ValidItem.IsValid())
	{
		// Swap with the over item.
		SwapWithHoverItem(ClickedInventoryItem, GridIndex);
	}
	
}

void UInv_InventoryGrid::CreateItemPopUp(const int32 GridIndex)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;
	if (IsValid(GridSlots[GridIndex]->GetItemPopUp())) return;

	ItemPopUp = CreateWidget<UInv_ItemPopUp>(this, ItemPopUpClass);
	GridSlots[GridIndex]->SetItemPopUp(ItemPopUp);
	
	OwningCanvasPanel->AddChild(ItemPopUp);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemPopUp);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	CanvasSlot->SetPosition(MousePosition - ItemPopUpOffset);
	CanvasSlot->SetSize(ItemPopUp->GetBoxSize());

	const int32 SliderMax = GridSlots[GridIndex]->GetStackCount() - 1;
	if ( RightClickedItem->IsStackable() && SliderMax > 0)
	{
		ItemPopUp->OnSplit.BindDynamic(this, &ThisClass::UInv_InventoryGrid::OnPopUpMenuSplit);
		ItemPopUp->SetSliderParameters(SliderMax, FMath::Max(1, GridSlots[GridIndex]->GetStackCount() / 2));
	}
	else
	{
		ItemPopUp->CollapseSplitButton();
	}

	ItemPopUp->OnDrop.BindDynamic(this, &ThisClass::UInv_InventoryGrid::OnPopUpMenuDrop);

	if ( RightClickedItem->IsConsumable())
	{
		ItemPopUp->OnConsume.BindDynamic(this, &ThisClass::UInv_InventoryGrid::OnPopUpMenuConsumed);
	}
	else
	{
		ItemPopUp->CollapseConsumeButton();
	}
	
}

void UInv_InventoryGrid::PutHoverItemBack()
{
	if (!IsValid(HoverItem)) return;
	FInv_SlotAvailabilityResult Result = HasRoomForItem(HoverItem->GetInventoryItem(), HoverItem->GetStackCount());
	Result.Item = HoverItem->GetInventoryItem();
	AddStacks(Result);
	ClearHoverItem();
}

void UInv_InventoryGrid::DropItem()
{
	if (!IsValid(HoverItem) || !IsValid(HoverItem->GetInventoryItem())) return;

	InventoryComponent->Server_DropItem(HoverItem->GetInventoryItem(), HoverItem->GetStackCount());
	ClearHoverItem();
}

bool UInv_InventoryGrid::HasHoverItem() const
{
	return IsValid(HoverItem);
}

UInv_HoverItem* UInv_InventoryGrid::GetHoverItem() const
{
	return HoverItem;
}

float UInv_InventoryGrid::GetTileSize() const
{
	return TileSize;
}

void UInv_InventoryGrid::PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	AssignHoverItem(ClickedInventoryItem, GridIndex, GridIndex);
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem)
{
	if (!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UInv_HoverItem>(GetOwningPlayer(), HoverItemClass);
	}

	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(InventoryItem, FragmentTags::ImageFragment);
	if (!GridFragment || !ImageFragment) return;

	const FVector2D DrawSize = GetDrawSize(GridFragment);

	FSlateBrush IconBrush;
	IconBrush.SetResourceObject(ImageFragment->GetIcon());
	IconBrush.DrawAs = ESlateBrushDrawType::Image;
	IconBrush.ImageSize = DrawSize * UWidgetLayoutLibrary::GetViewportScale(this);

	HoverItem->SetImageBrush(IconBrush);
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());

	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, HoverItem);
}

void UInv_InventoryGrid::OnHide()
{
	PutHoverItemBack();
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem, const int32 GridIndex,
                                         const int32 PreviousGridIndex)
{
	AssignHoverItem(InventoryItem);

	HoverItem->SetPreviousGridIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ? GridSlots[GridIndex]->GetStackCount() : 0);
}

void UInv_InventoryGrid::RemoveItemFromGrid(UInv_InventoryItem* InventoryItem, const int32 GridIndex)
{
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	if (!GridFragment) return;

	UInv_InventoryStatics::ForEach2D(GridSlots, GridIndex, GridFragment->GetGridSize(), Columns, [&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(nullptr);
		GridSlot->SetUpperLeftIndex(INDEX_NONE);
		GridSlot->SetUnoccupiedTexture();
		GridSlot->SetIsAvailable(true);
		GridSlot->SetStackCount(0);
	});

	if (SlottedItems.Contains(GridIndex))
	{
		TObjectPtr<UInv_SlottedItem> FoundSlottedItem;
		SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
}

void UInv_InventoryGrid::UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition)
{

	if (!bMouseWithinCanvas) return;
	
	const FIntPoint HoveredTileCoordinates = CalculateHoveredCoordinates(CanvasPosition, MousePosition);

	LastTileParameters = TileParameters;
	TileParameters.TileCoordinates = HoveredTileCoordinates;
	TileParameters.TileIndex = UInv_WidgetUtils::GetIndexFromPosition(HoveredTileCoordinates, Columns);
	TileParameters.TileQuadrant = CalculateTileQuadrant(CanvasPosition, MousePosition);
	OnTileParametersUpdated(TileParameters);
}

FIntPoint UInv_InventoryGrid::CalculateHoveredCoordinates(const FVector2D& CanvasPosition,
	const FVector2D& MousePosition) const
{
	const FIntPoint Coordinates
	{
		static_cast<int32>(FMath::FloorToInt ((MousePosition.X - CanvasPosition.X) / TileSize)),
		static_cast<int32>(FMath::FloorToInt ((MousePosition.Y - CanvasPosition.Y) / TileSize))
	};
	return Coordinates;
}

EInv_TileQuadrant UInv_InventoryGrid::CalculateTileQuadrant(const FVector2D& CanvasPosition,
	const FVector2D& MousePosition) const
{
	const float TileLocalX = FMath::Fmod(MousePosition.X - CanvasPosition.X, TileSize);
	const float TileLocalY = FMath::Fmod(MousePosition.Y - CanvasPosition.Y, TileSize);

	const bool bIsTop = TileLocalY < TileSize / 2;  // Top if Y is in the upper half
	const bool bIsLeft = TileLocalX < TileSize / 2; // Left if x is in the left half

	EInv_TileQuadrant HoveredTileQuadrant = EInv_TileQuadrant::None;
	if (bIsTop && bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::TopLeft;
	else if (bIsTop && !bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::TopRight;
	else if (!bIsTop && bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::BottomLeft;
	else if (!bIsTop && !bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::BottomRight;

	return HoveredTileQuadrant;
}

void UInv_InventoryGrid::OnTileParametersUpdated(const FInv_TileParameters& NewTileParameters)
{
	if (!IsValid(HoverItem)) return;

	// Get Hover Item's Dimensions
	const FIntPoint Dimensions = HoverItem->GetGridDimensions();
	// calculate the starting coordinate for highlighting
	const FIntPoint StartingCoordinate = CalculateStartingCoordinate(NewTileParameters.TileCoordinates, Dimensions, NewTileParameters.TileQuadrant);
	ItemDropIndex = UInv_WidgetUtils::GetIndexFromPosition(StartingCoordinate, Columns);
	
	CurrentQueryResult = CheckHoverPosition(StartingCoordinate, Dimensions);

	if (CurrentQueryResult.bHasSpace)
	{
		HighlightSlots(ItemDropIndex, Dimensions);
		return;
	}
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);

	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(CurrentQueryResult.ValidItem.Get(), FragmentTags::GridFragment);
		if (!GridFragment) return;

		ChangeHoverType(CurrentQueryResult.UpperLeftIndex, GridFragment->GetGridSize(), EInv_GridSlotState::GrayedOut);
	}
	
}

FIntPoint UInv_InventoryGrid::CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions,
	const EInv_TileQuadrant Quadrant) const
{
	const int32 HasEvenWidth = Dimensions.X % 2 == 0 ? 1 : 0;
	const int32 HasEvenHeight = Dimensions.Y % 2 == 0 ? 1 : 0;

	FIntPoint StartingCoordinate;
	switch (Quadrant)
	{
		case EInv_TileQuadrant::TopLeft:
			StartingCoordinate.X =  Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
			StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
		
		case EInv_TileQuadrant::TopRight:
			StartingCoordinate.X =  Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
			StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;

		case EInv_TileQuadrant::BottomLeft:
			StartingCoordinate.X =  Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
			StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;

		case EInv_TileQuadrant::BottomRight:
			StartingCoordinate.X =  Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
			StartingCoordinate.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
		
		default:
			UE_LOG(LogInventory, Error, TEXT("Invalid Tile Quadrant"));
			return FIntPoint{-1, -1};
	}
	return StartingCoordinate;
}

FInv_SpaceQueryResult UInv_InventoryGrid::CheckHoverPosition(const FIntPoint& Position,
	const FIntPoint& Dimensions) 
{

	FInv_SpaceQueryResult Result {};

	if (!IsInGridBounds(UInv_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions)) return Result;

	Result.bHasSpace = true;
	// If more than one of the indices is occupied with the same item, we need to see if they all have the same upper-left index.
	// If all the upper-left indexes are the same, then they all are the same GridSlot item.  If different, they are different GridSlot Items.
	TSet<int32> OccupiedUpperLeftIndices;
	UInv_InventoryStatics::ForEach2D(GridSlots, UInv_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		if (GridSlot->GetInventoryItem().IsValid())
		{
			OccupiedUpperLeftIndices.Add(GridSlot->GetUpperLeftIndex());
			Result.bHasSpace = false;
		}
	});
	
	// if so, is there only one time in the way? (can we swap?)
	if (OccupiedUpperLeftIndices.Num() == 1) // Single item at position - it's valid for swapping / combining
	{
		const int32 Index = *OccupiedUpperLeftIndices.CreateConstIterator();
		Result.ValidItem = GridSlots[Index]->GetInventoryItem();
		Result.UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	}
	return Result;
}

bool UInv_InventoryGrid::CursorExitedCanvas(const FVector2D& CanvasPosition, const FVector2D& BoundarySize,
	const FVector2D& MouseLocation)
{
	bLastMouseWithinCanvas = bMouseWithinCanvas;
	bMouseWithinCanvas = UInv_WidgetUtils::IsWithinBounds(CanvasPosition, BoundarySize, MouseLocation);
	// Just exited a canvas panel.
	if ( !bMouseWithinCanvas && bLastMouseWithinCanvas)
	{
		UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
		return true;
	}
	return false;
}

void UInv_InventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	if (!bMouseWithinCanvas) return;
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetOccupiedTexture();
	});

	LastHighlightedDimensions = Dimensions;
	LastHighlightedIndex = Index;	
}

void UInv_InventoryGrid::UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
{
	if (GridSlot->GetIsAvailable())
	{
		GridSlot->SetUnoccupiedTexture();
	}
	else // Slot already had an item, so don't warn to change anything.
	{
		GridSlot->SetOccupiedTexture();
	}
});

}

void UInv_InventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimensions,
	EInv_GridSlotState GridSlotState)
{
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		switch (GridSlotState)
		{
			case EInv_GridSlotState::Occupied:
				GridSlot->SetOccupiedTexture();
				break;
			case EInv_GridSlotState::Unoccupied:
				GridSlot->SetUnoccupiedTexture();
				break;
			case EInv_GridSlotState::GrayedOut:
				GridSlot->SetGrayedOutTexture();
				break;
			case EInv_GridSlotState::Selected:
				GridSlot->SetSelectedTexture();
				break;
			default:
				UE_LOG(LogInventory, Error, TEXT("Invalid GridSlotState"));
		}
	});

	LastHighlightedDimensions = Dimensions;
	LastHighlightedIndex = Index;
}

void UInv_InventoryGrid::PutDownOnIndex(const int32 Index)
{
	if (!IsValid(HoverItem)) return;
	if (!GridSlots.IsValidIndex(Index)) return;
	AddItemAtIndex(HoverItem->GetInventoryItem(), Index, HoverItem->GetIsStackable(), HoverItem->GetStackCount());
	UpdateGridSlots(HoverItem->GetInventoryItem(), Index, HoverItem->GetIsStackable(), HoverItem->GetStackCount());
	ClearHoverItem();
	
}

void UInv_InventoryGrid::ClearHoverItem()
{
	if (!IsValid(HoverItem)) return;
	HoverItem->SetInventoryItem(nullptr);
	HoverItem->SetIsStackable(false);
	HoverItem->SetPreviousGridIndex(INDEX_NONE);
	HoverItem->UpdateStackCount(0);
	HoverItem->SetImageBrush(FSlateNoResource());

	HoverItem->RemoveFromParent();
	HoverItem = nullptr;

	ShowCursor();
}

UUserWidget* UInv_InventoryGrid::GetVisibleCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(VisibleCursorWidget))
	{
		VisibleCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), VisibleCursorWidgetClass);
	}
	return VisibleCursorWidget;
}

UUserWidget* UInv_InventoryGrid::GetHiddenCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(HiddenCursorWidget))
	{
		HiddenCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), HiddenCursorWidgetClass);
	}
	return HiddenCursorWidget;
}

bool UInv_InventoryGrid::IsSameStackable(const UInv_InventoryItem* ClickedInventoryItem) const
{
	const bool bIsSameItem = ClickedInventoryItem == HoverItem->GetInventoryItem();
	const bool bIsStackable = ClickedInventoryItem->IsStackable();
	return bIsSameItem && bIsStackable && HoverItem->GetItemType().MatchesTagExact(ClickedInventoryItem->GetItemManifest().GetItemType());

}

void UInv_InventoryGrid::SwapWithHoverItem(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	if (!IsValid(HoverItem)) return;
	if (!IsValid(ClickedInventoryItem)) return;

	UInv_InventoryItem* TempInventoryItem = HoverItem->GetInventoryItem();
	const int32 TempStackCount = HoverItem->GetStackCount();
	const bool bTempIsStackable = HoverItem->GetIsStackable();

	// Keep the same previous grid index.
	AssignHoverItem(ClickedInventoryItem, GridIndex, HoverItem->GetPreviousGridIndex());
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
	AddItemAtIndex(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
	UpdateGridSlots(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
}

bool UInv_InventoryGrid::ShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoveredStackCount,
	const int32 MaxStackSize) const
{
	return RoomInClickedSlot == 0 && HoveredStackCount < MaxStackSize;
}

void UInv_InventoryGrid::SwapStackCounts(const int32 ClickedStackCount, const int32 HoveredStackCount,
	const int32 Index)
{
	UInv_GridSlot* GridSlot = GridSlots[Index];

	GridSlot->SetStackCount(HoveredStackCount);
	UInv_SlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(HoveredStackCount);

	HoverItem->UpdateStackCount(ClickedStackCount);
	
}

bool UInv_InventoryGrid::ShouldConsumeHoverItemStacks(const int32 HoveredStackCount,
	const int32 RoomInClickedSlot) const
{
	return RoomInClickedSlot >= HoveredStackCount;
}

void UInv_InventoryGrid::ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoveredStackCount,
	const int32 Index)
{
	const int32 AmountToTransfer = HoveredStackCount;
	const int32 NewClickedStackCount = ClickedStackCount + AmountToTransfer;

	GridSlots[Index]->SetStackCount(NewClickedStackCount);
	SlottedItems.FindChecked(Index)->UpdateStackCount(NewClickedStackCount);
	ClearHoverItem();
	ShowCursor();

	const FInv_GridFragment* GridFragment = GridSlots[Index]->GetInventoryItem()->GetItemManifest().GetFragmentOfType<FInv_GridFragment>();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
	HighlightSlots(Index, Dimensions);
}

bool UInv_InventoryGrid::ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount)
{
	return RoomInClickedSlot < HoveredStackCount;
}

void UInv_InventoryGrid::FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index)
{
	UInv_GridSlot* GridSlot = GridSlots[Index];
	const int32 NewStackCount = GridSlot->GetStackCount() + FillAmount;

	GridSlot->SetStackCount(NewStackCount);

	UInv_SlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(NewStackCount);

	HoverItem->UpdateStackCount(Remainder);
}

void UInv_InventoryGrid::ShowCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetVisibleCursorWidget());
}

void UInv_InventoryGrid::HideCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetHiddenCursorWidget());
}

void UInv_InventoryGrid::SetOwningCanvasPanel(UCanvasPanel* OwningCanvas)
{
	OwningCanvasPanel = OwningCanvas;
}


void UInv_InventoryGrid::AddItem(UInv_InventoryItem* Item)
{
	if (!MatchesCategory(Item)) return;
	FInv_SlotAvailabilityResult Result = HasRoomForItem(Item);
	AddItemToIndices(Result, Item);
	
}

void UInv_InventoryGrid::AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewItem)
{

	for (const auto& Availability : Result.SlotAvailabilities )
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
		UpdateGridSlots(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
	}
}

void UInv_InventoryGrid::AddItemAtIndex(UInv_InventoryItem* Item, const int32 Index, const bool bStackable,
	const int32 StackAmount)
{
	
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(Item, FragmentTags::GridFragment);
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(Item, FragmentTags::ImageFragment);
	if (!GridFragment || !ImageFragment) return;
	UInv_SlottedItem* SlottedItem = CreateSlottedItem(Item, bStackable, StackAmount, GridFragment, ImageFragment, Index);
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);
	SlottedItems.Add(Index, SlottedItem);
}


void UInv_InventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);

	for ( int32 j = 0; j < Rows; j++ )
	{
		for (int32 i = 0; i < Columns; i++)
		{
			UInv_GridSlot*  NewSlot = CreateWidget<UInv_GridSlot>(this, GridSlotClass);
			CanvasPanel->AddChildToCanvas(NewSlot);

			const FIntPoint TilePosition = FIntPoint(i, j);
			NewSlot->SetIndex( UInv_WidgetUtils::GetIndexFromPosition(TilePosition, Columns));

			UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(NewSlot);
			CanvasSlot->SetSize(FVector2D(TileSize));
			CanvasSlot->SetPosition(TilePosition * TileSize);

			GridSlots.Add(NewSlot);
			NewSlot->OnGridSlotClicked.AddDynamic(this, &ThisClass::OnGridSlotClicked);
			NewSlot->OnGridSlotHovered.AddDynamic(this, &ThisClass::OnGridSlotHovered);
			NewSlot->OnGridSlotUnhovered.AddDynamic(this, &ThisClass::OnGridSlotUnhovered);
		}
	}
}

void UInv_InventoryGrid::OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (!IsValid(HoverItem)) return;
	if (!GridSlots.IsValidIndex(ItemDropIndex)) return;

	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex, MouseEvent);
		return;
	}

	auto GridSlot = GridSlots[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid())
	{
		PutDownOnIndex(ItemDropIndex);
	}
}

void UInv_InventoryGrid::OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;
	UInv_GridSlot* GridSlot = GridSlots[GridIndex];
	if ( GridSlot->GetIsAvailable())
	{
		GridSlot->SetOccupiedTexture();
	}
	
}

void UInv_InventoryGrid::OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;
	UInv_GridSlot* GridSlot = GridSlots[GridIndex];
	if ( GridSlot->GetIsAvailable())
	{
		GridSlot->SetUnoccupiedTexture();
	}
}

void UInv_InventoryGrid::OnPopUpMenuSplit(int32 SplitAmount, int32 Index)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem) || !RightClickedItem->IsStackable()) return;

	const int32 UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	UInv_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
	const int32 StackCount = UpperLeftGridSlot->GetStackCount();
	const int32 NewStackCount = StackCount - SplitAmount;

	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);

	AssignHoverItem(RightClickedItem, UpperLeftIndex, UpperLeftIndex);
	HoverItem->UpdateStackCount(SplitAmount);
}

void UInv_InventoryGrid::OnPopUpMenuDrop(int32 Index)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;

	PickUp(RightClickedItem, Index);
	DropItem();
}

void UInv_InventoryGrid::OnPopUpMenuConsumed(int32 Index)
{
	UInv_InventoryItem* RightClickedItem = GridSlots[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedItem)) return;

	const int32 UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	UInv_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
	const int32 StackCount = UpperLeftGridSlot->GetStackCount();
	const int32 NewStackCount = StackCount - 1;

	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);

	InventoryComponent->Server_ConsumeItem(RightClickedItem);

	if ( NewStackCount <= 0)
	{
		RemoveItemFromGrid(RightClickedItem, UpperLeftIndex);
	}
}

void UInv_InventoryGrid::OnInventoryMenuToggled(bool bOpen)
{
	if (!bOpen)
	{
		PutHoverItemBack();
	}
}


bool UInv_InventoryGrid::MatchesCategory(const UInv_InventoryItem* Item) const
{
	return Item->GetItemManifest().GetItemCategory() == ItemCategory;
}

FVector2D UInv_InventoryGrid::GetDrawSize(const FInv_GridFragment* GridFragment) const
{
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	const FVector2D IconSize = GridFragment->GetGridSize() * IconTileWidth;
	return IconSize;
}

void UInv_InventoryGrid::SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment,
	const FInv_ImageFragment* ImageFragment) const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}


UInv_SlottedItem* UInv_InventoryGrid::CreateSlottedItem(UInv_InventoryItem* Item, const bool bStackable,
	const int32 StackAmount, const FInv_GridFragment* GridFragment, const FInv_ImageFragment* ImageFragment,
	const int32 Index)
{
	UInv_SlottedItem* SlottedItem = CreateWidget<UInv_SlottedItem>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetGridIndex(Index);
	SlottedItem->SetIsStackable(bStackable);
	const int32 StackCount = bStackable ? StackAmount : 0;
	SlottedItem->UpdateStackCount(StackCount);
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &ThisClass::OnSlottedItemClicked);
	return SlottedItem;
}

void UInv_InventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FInv_GridFragment* GridFragment,
	UInv_SlottedItem* SlottedItem) const
{
	CanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));
	const FVector2D DrawPosition = UInv_WidgetUtils::GetPositionFromIndex(Index, Columns) * TileSize;
	const FVector2D DrawPositionWithPadding = DrawPosition + FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(DrawPositionWithPadding);
	
}

void UInv_InventoryGrid::UpdateGridSlots(UInv_InventoryItem* NewItem, const int32 Index, bool bStackable, const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));

	if (bStackable)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}
	
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(NewItem, FragmentTags::GridFragment);
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);

	UInv_InventoryStatics::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(NewItem);
		GridSlot->SetUpperLeftIndex(Index);
		GridSlot->SetOccupiedTexture();
		GridSlot->SetIsAvailable(false);
	});
	
	UInv_GridSlot* GridSlot = GridSlots[Index];
	GridSlot->SetOccupiedTexture();
}

bool UInv_InventoryGrid::IsIndexClaimed(const TSet<int32>& CheckIndices, const int32 Index) const
{
	return CheckIndices.Contains(Index);
}


