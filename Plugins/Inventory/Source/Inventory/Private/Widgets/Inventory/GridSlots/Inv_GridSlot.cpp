// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Widgets/Inventory/GridSlots/Inv_GridSlot.h"
#include "Items/Inv_InventoryItem.h" 
#include "Components/Image.h"
#include "Widgets/ItemPopUp/Inv_ItemPopUp.h"

void UInv_GridSlot::SetIndex(int32 Index)
{
	TileIndex = Index;
}

void UInv_GridSlot::SetInventoryItem(UInv_InventoryItem* Item)
{
	InventoryItem = Item;
}

void UInv_GridSlot::SetUpperLeftIndex(int32 Index)
{
	UpperLeftIndex = Index;
}

void UInv_GridSlot::SetStackCount(int32 Count)
{
	StackCount = Count;
}

UInv_ItemPopUp* UInv_GridSlot::GetItemPopUp() const
{
	return ItemPopUp.Get();
}

void UInv_GridSlot::SetItemPopUp(UInv_ItemPopUp* PopUp)
{
	ItemPopUp = PopUp;
	ItemPopUp->SetGridIndex(GetIndex());
	ItemPopUp->OnNativeDestruct.AddUObject(this, &ThisClass::OnItemPopUpDestruct);
}

void UInv_GridSlot::OnItemPopUpDestruct(UUserWidget* Menu)
{
	ItemPopUp.Reset();
}

void UInv_GridSlot::SetOccupiedTexture()
{
	GridSlotState = EInv_GridSlotState::Occupied;
	Image_GridSlot->SetBrush(Brush_Occupied);
}

void UInv_GridSlot::SetUnoccupiedTexture()
{
	GridSlotState = EInv_GridSlotState::Unoccupied;
	Image_GridSlot->SetBrush(Brush_Unoccupied);
}

void UInv_GridSlot::SetSelectedTexture()
{
	GridSlotState = EInv_GridSlotState::Selected;
	Image_GridSlot->SetBrush(Brush_Selected);
}

void UInv_GridSlot::SetGrayedOutTexture()
{
	GridSlotState = EInv_GridSlotState::GrayedOut;
	Image_GridSlot->SetBrush(Brush_GreyedOut);
}
