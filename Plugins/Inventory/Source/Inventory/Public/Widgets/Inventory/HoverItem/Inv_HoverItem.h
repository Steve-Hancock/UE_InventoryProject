// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Inv_HoverItem.generated.h"

class UTextBlock;
class UInv_InventoryItem;
class UImage;
/**
 *  The HoverItem is the item that will appear and follow the mouse when the inventory Item on the grid has been clicked.
 */
UCLASS()
class INVENTORY_API UInv_HoverItem : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetImageBrush(const FSlateBrush& Brush) const;
	void UpdateStackCount(int32 StackCount);
	void SetIsStackable(bool bStackable);
	void SetPreviousGridIndex(int32 Index) {PreviousGridIndex = Index;}
	void SetGridDimensions(const FIntPoint& Dimensions) {GridDimensions = Dimensions;}
	void SetInventoryItem(UInv_InventoryItem* Item);

	FGameplayTag GetItemType() const;
	int32 GetStackCount() const { return StackCount;}
	bool GetIsStackable() const { return bIsStackable; }
	int32 GetPreviousGridIndex() const { return PreviousGridIndex; }
	FIntPoint GetGridDimensions() const { return GridDimensions; }
	UInv_InventoryItem* GetInventoryItem() const;
	
	
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	int32 PreviousGridIndex;
	FIntPoint GridDimensions;

	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	bool bIsStackable{false};
	int32 StackCount{0};
};
