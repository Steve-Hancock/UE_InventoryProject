// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/Inv_InventoryItem.h"

void UInv_HoverItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}

void UInv_HoverItem::UpdateStackCount(int32 Count)
{
	StackCount = Count;
	if (StackCount > 0)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		Text_StackCount->SetText(FText::AsNumber(StackCount));
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInv_HoverItem::SetIsStackable(bool bStackable)
{
	bIsStackable = bStackable;
	if ( !bIsStackable)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInv_HoverItem::SetInventoryItem(UInv_InventoryItem* Item)
{
	InventoryItem = Item;
}

FGameplayTag UInv_HoverItem::GetItemType() const
{
	if (InventoryItem.IsValid())
	{
		return InventoryItem->GetItemManifest().GetItemType();
	}

	return FGameplayTag();
}

UInv_InventoryItem* UInv_HoverItem::GetInventoryItem() const
{
	return InventoryItem.Get();
}
