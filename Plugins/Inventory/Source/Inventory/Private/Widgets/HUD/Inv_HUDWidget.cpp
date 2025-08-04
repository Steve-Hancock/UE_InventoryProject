// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Widgets/HUD/Inv_HUDWidget.h"

#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Widgets/HUD/Inv_InfoMessage.h"

void UInv_HUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UInv_InventoryComponent* InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->NoRoomInInventory.AddDynamic(this, &ThisClass::OnNoRoomInInventory);
	}
}

void UInv_HUDWidget::OnNoRoomInInventory()
{
	if (!IsValid(InfoMessage)) return;
	static const FText NoRoomMessage = FText::FromString("No Room in Inventory");
	InfoMessage->SetMessage(NoRoomMessage);
}
