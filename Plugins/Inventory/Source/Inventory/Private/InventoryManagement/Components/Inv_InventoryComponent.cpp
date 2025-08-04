// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Widgets/Inventory/InventoryBase/Inv_InventoryBase.h"
#include "Blueprint/UserWidget.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Player/Inv_PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Items/Inv_InventoryItem.h"

UInv_InventoryComponent::UInv_InventoryComponent()
	:
		InventoryList(this)
{

	PrimaryComponentTick.bCanEverTick = false;
	bInventoryMenuOpen = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
	
}

void UInv_InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, InventoryList);
}

void UInv_InventoryComponent::TryAddItem(UInv_ItemComponent* ItemComponent)
{
	FInv_SlotAvailabilityResult Result = InventoryMenu->HasRoomForItem(ItemComponent);

	UInv_InventoryItem* FoundItem = InventoryList.FindFirstItemByType(ItemComponent->GetItemManifest().GetItemType());
	Result.Item	= FoundItem;

	if (Result.TotalRoomToFill == 0)
	{
		NoRoomInInventory.Broadcast();
		return;
	}
	
	if ( Result.Item.IsValid() && Result.bStackable )
	{
		// Add stacks to an item that already exists in the inventory. We only want to update the stack count.
		// Not create a new item of this type.
		OnStackChange.Broadcast(Result);
		Server_AddStacksToItem(ItemComponent, Result.TotalRoomToFill, Result.Remainder);
	}
	else if (Result.TotalRoomToFill > 0)
	{
		// This Item type doesn't exist in the inventory. Create a new one and update all pertinent slots.
		Server_AddNewItem(ItemComponent, Result.bStackable? Result.TotalRoomToFill : 0);
	}
}

void UInv_InventoryComponent::Server_AddNewItem_Implementation(UInv_ItemComponent* ItemComponent, int32 StackCount)
{
	UInv_InventoryItem* NewItem = InventoryList.AddEntry(ItemComponent);
	NewItem->SetTotalStackCount(StackCount);

	if (GetOwner()->GetNetMode() == NM_ListenServer || GetOwner()->GetNetMode() == NM_Standalone)
	{
		OnItemAdded.Broadcast(NewItem);
	}
	
	ItemComponent->PickedUp();
}

void UInv_InventoryComponent::Server_AddStacksToItem_Implementation(UInv_ItemComponent* ItemComponent, int32 StackCount,
	int32 Remainder)
{
	const FGameplayTag& ItemType =  IsValid(ItemComponent) ? ItemComponent->GetItemManifest().GetItemType() : FGameplayTag::EmptyTag;
	UInv_InventoryItem* Item = InventoryList.FindFirstItemByType(ItemType);
	if (!IsValid(Item)) return;
	Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);

	if (Remainder == 0)
	{
		ItemComponent->PickedUp();	
	}
	else if (FInv_StackableFragment* StackableFragment = ItemComponent->GetItemManifest().GetFragmentOfTypeMutable<FInv_StackableFragment>())
	{
		StackableFragment->SetStackCount(Remainder);
	}
}

void UInv_InventoryComponent::Server_DropItem_Implementation(UInv_InventoryItem* Item, int32 StackCount)
{
	const int32 newStackCount = Item->GetTotalStackCount() - StackCount;
	if (newStackCount == 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(newStackCount);
	}

	SpawnDroppedItem(Item, StackCount);
}

void UInv_InventoryComponent::Server_ConsumeItem_Implementation(UInv_InventoryItem* Item)
{
	const int32 newStackCount = Item->GetTotalStackCount() - 1;
	if (newStackCount == 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(newStackCount);
	}

	if (FInv_ConsumableFragment* ConsumableFragment = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FInv_ConsumableFragment>())
	{
		ConsumableFragment->OnConsume(OwningController.Get(), Item);
	}
}

void UInv_InventoryComponent::Server_EquipSlotClicked_Implementation(UInv_InventoryItem* ItemToEquip,
	UInv_InventoryItem* ItemToUnEquip)
{
	Multicast_EquipSlotClicked(ItemToEquip, ItemToUnEquip);
}

void UInv_InventoryComponent::Multicast_EquipSlotClicked_Implementation(UInv_InventoryItem* ItemToEquip,
	UInv_InventoryItem* ItemToUnEquip)
{
	// Equipment Component will listen to these delegates
	OnItemEquipped.Broadcast(ItemToEquip);
	OnItemUnequipped.Broadcast(ItemToUnEquip);
}

void UInv_InventoryComponent::ToggleInventoryMenu()
{
	if (bInventoryMenuOpen)
	{
		CloseInventoryMenu();
	}
	else
	{
		OpenInventoryMenu();
	}
	OnInventoryMenuToggled.Broadcast(bInventoryMenuOpen);
}

void UInv_InventoryComponent::AddRepSubObject(UObject* SubObject)
{
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && IsValid(SubObject))
	{
		AddReplicatedSubObject(SubObject);
	}
}

void UInv_InventoryComponent::SpawnDroppedItem(UInv_InventoryItem* Item, int32 StackCount)
{
	const APawn* OwningPawn = OwningController->GetPawn();

	FVector RotatedForward = OwningPawn->GetActorForwardVector();
	RotatedForward = RotatedForward.RotateAngleAxis(FMath::FRandRange(DropSpawnAngleMin, DropSpawnAngleMax), FVector::UpVector);
	FVector SpawnLocation = OwningPawn->GetActorLocation() + RotatedForward * FMath::FRandRange(DropSpawnDistanceMin, DropSpawnDistanceMax);
	SpawnLocation.Z -= 100.f;
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FInv_ItemManifest& Manifest = Item->GetItemManifestMutable();
	if (FInv_StackableFragment* StackableFragment = Manifest.GetFragmentOfTypeMutable<FInv_StackableFragment>())
	{
		StackableFragment->SetStackCount(StackCount);
	}
	Manifest.SpawnPickupActor(this, SpawnLocation, SpawnRotation);
}


void UInv_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	ConstructInventory();
}

void UInv_InventoryComponent::ConstructInventory()
{
	OwningController = Cast<AInv_PlayerController>(GetOwner());
	checkf(OwningController.IsValid(), TEXT("Inventory Component should have a Player Controller as Owner"))
	if (!OwningController->IsLocalController()) return;

	InventoryMenu = CreateWidget<UInv_InventoryBase>(OwningController.Get(), InventoryMenuClass);
	InventoryMenu->AddToViewport();
	CloseInventoryMenu();
}

void UInv_InventoryComponent::OpenInventoryMenu()
{
	if (!IsValid(InventoryMenu)) return;
	InventoryMenu->SetVisibility(ESlateVisibility::Visible);

	if (!OwningController.IsValid()) return;
	FInputModeGameAndUI InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(true);
	bInventoryMenuOpen = true;
}

void UInv_InventoryComponent::CloseInventoryMenu()
{
	if (!IsValid(InventoryMenu)) return;
	InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);

	if (!OwningController.IsValid()) return;
	FInputModeGameOnly InputMode;
	OwningController->SetInputMode(InputMode);
	OwningController->SetShowMouseCursor(false);
	bInventoryMenuOpen = false;
}


