// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayCueNotify_Static.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"

UInv_InventoryComponent* UInv_InventoryStatics::GetInventoryComponent(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return nullptr;
	UInv_InventoryComponent* InventoryComponent = PlayerController->FindComponentByClass<UInv_InventoryComponent>();
	return InventoryComponent;
}

EInv_ItemCategory UInv_InventoryStatics::GetItemCategoryFromItemComponent(UInv_ItemComponent* ItemComponent)
{
	if (!IsValid(ItemComponent)) return EInv_ItemCategory::None;
	return ItemComponent->GetItemManifest().GetItemCategory();
}

void UInv_InventoryStatics::ItemHovered(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	UInv_InventoryComponent* InventoryComponent = GetInventoryComponent(PlayerController);
	if (!IsValid(InventoryComponent)) return;

	UInv_InventoryBase* InventoryBase = InventoryComponent->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return;

	if (InventoryBase->HasHoverItem()) return;

	InventoryBase->OnItemHovered(Item);
}

void UInv_InventoryStatics::ItemUnHovered(APlayerController* PlayerController)
{
	UInv_InventoryComponent* InventoryComponent = GetInventoryComponent(PlayerController);
	if (!IsValid(InventoryComponent)) return;

	UInv_InventoryBase* InventoryBase = InventoryComponent->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return;

	InventoryBase->OnItemUnHovered();
}

UInv_HoverItem* UInv_InventoryStatics::GetHoverItem(APlayerController* PlayerController)
{
	UInv_InventoryComponent* InventoryComponent = GetInventoryComponent(PlayerController);
	if (!IsValid(InventoryComponent)) return nullptr;

	UInv_InventoryBase* InventoryBase = InventoryComponent->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return nullptr;

	return InventoryBase->GetHoverItem();
}

UInv_InventoryBase* UInv_InventoryStatics::GetInventoryWidget(APlayerController* PlayerController)
{
	UInv_InventoryComponent* InventoryComponent = GetInventoryComponent(PlayerController);
	if (!IsValid(InventoryComponent)) return nullptr;

	return InventoryComponent->GetInventoryMenu();
}

UAbilitySystemComponent* UInv_InventoryStatics::GetAbilitySystemComponent(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return nullptr;
	return UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerController->GetPawn());
}


