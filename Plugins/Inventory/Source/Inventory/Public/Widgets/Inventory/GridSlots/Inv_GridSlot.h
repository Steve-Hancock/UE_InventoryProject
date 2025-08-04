﻿// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/ItemPopUp/Inv_ItemPopUp.h"
#include "Inv_GridSlot.generated.h"

class UInv_ItemPopUp;
class UInv_InventoryItem;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridSlotEvent, int32, GridIndex, const FPointerEvent&, MouseEvent);

UENUM(BlueprintType)
enum class EInv_GridSlotState : uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GrayedOut
};

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_GridSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		Super::NativeOnMouseEnter(MyGeometry, MouseEvent);
		OnGridSlotHovered.Broadcast(TileIndex, MouseEvent);
	}

	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override
	{
		Super::NativeOnMouseLeave(MouseEvent);
		OnGridSlotUnhovered.Broadcast(TileIndex, MouseEvent);
	}

	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override
	{
		OnGridSlotClicked.Broadcast(TileIndex, MouseEvent);
		return FReply::Handled();
		
	}

	void SetIndex(int32 Index);
	void SetInventoryItem(UInv_InventoryItem* Item);
	void SetUpperLeftIndex(int32 Index);
	void SetStackCount(int32 Count);
	void SetIsAvailable(bool bAvailable){bIsAvailable = bAvailable;};
	
	int32 GetIndex() const { return TileIndex; };
	EInv_GridSlotState GetGridSlotState() const {return GridSlotState; }
	TWeakObjectPtr<UInv_InventoryItem> GetInventoryItem() const {return InventoryItem; }
	int32 GetUpperLeftIndex() const {return UpperLeftIndex; }
	int32 GetStackCount() const {return StackCount; }
	bool GetIsAvailable() const {return bIsAvailable; }
	UInv_ItemPopUp* GetItemPopUp() const;
	

	void SetOccupiedTexture();
	void SetUnoccupiedTexture();
	void SetSelectedTexture();
	void SetGrayedOutTexture();
	void SetItemPopUp(UInv_ItemPopUp* PopUp);

	FGridSlotEvent OnGridSlotClicked;
	FGridSlotEvent OnGridSlotHovered;
	FGridSlotEvent OnGridSlotUnhovered;

private:
	int32 TileIndex{INDEX_NONE};
	int32 UpperLeftIndex{INDEX_NONE};
	
	int32 StackCount{0};
	bool bIsAvailable{true};

	TWeakObjectPtr<UInv_InventoryItem> InventoryItem {nullptr};
	TWeakObjectPtr<UInv_ItemPopUp> ItemPopUp {nullptr};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Unoccupied;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Occupied;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Selected;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_GreyedOut;

	EInv_GridSlotState GridSlotState;

	UFUNCTION()
	void OnItemPopUpDestruct(UUserWidget* Menu);

	
};
