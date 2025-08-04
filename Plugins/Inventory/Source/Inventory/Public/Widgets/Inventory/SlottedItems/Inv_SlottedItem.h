// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_SlottedItem.generated.h"

class UTextBlock;
class UInv_InventoryItem;
class UImage;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSlottedItemClicked, int32, GridIndex, const FPointerEvent&, MouseEvent);

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_SlottedItem : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	void SetIsStackable(bool bStackable) {bIsStackable = bStackable;}
	void SetGridIndex(int32 Index) {GridIndex = Index;}
	void SetGridDimensions(const FIntPoint& Dimensions) {GridDimensions = Dimensions;}
	void SetImage_Icon(UImage* Image) {Image_Icon = Image;}
	void SetInventoryItem(UInv_InventoryItem* Item);



	UImage* GetImage_Icon() const { return Image_Icon; }
	int32 GetGridIndex() const { return GridIndex; }
	FIntPoint GetGridDimensions() const { return GridDimensions; }
	bool GetIsStackable() const { return bIsStackable; }
	UInv_InventoryItem* GetInventoryItem()const {return InventoryItem.Get();};


	void SetImageBrush(const FSlateBrush& Brush) const;
	void UpdateStackCount(int32 StackCount);

	FSlottedItemClicked OnSlottedItemClicked;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	int32 GridIndex;
	FIntPoint GridDimensions;
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	bool bIsStackable{false};
};
