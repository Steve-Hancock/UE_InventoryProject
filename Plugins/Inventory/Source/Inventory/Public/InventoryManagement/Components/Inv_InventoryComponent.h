// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryManagement/FastArray/Inv_FastArray.h"
#include "Inv_InventoryComponent.generated.h"

struct FInv_SlotAvailabilityResult;
class UInv_InventoryItem;
class UInv_InventoryBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChange, UInv_InventoryItem*, Item );
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNoRoomInInventory);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStackChange, const FInv_SlotAvailabilityResult&, Result );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemEquipStatusChanged, UInv_InventoryItem*, Item );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryMenuToggled, bool, bOpen );

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UInv_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInv_InventoryComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Should only be called on the server.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void TryAddItem(UInv_ItemComponent* ItemComponent);

	UFUNCTION(Server, Reliable)
	void Server_AddNewItem(UInv_ItemComponent* ItemComponent, int32 StackCount);

	UFUNCTION(Server, Reliable)
	void Server_AddStacksToItem(UInv_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder);

	UFUNCTION(Server, Reliable)
	void Server_DropItem(UInv_InventoryItem* Item, int32 StackCount);

	UFUNCTION(Server, Reliable)
	void Server_ConsumeItem(UInv_InventoryItem* Item);

	UFUNCTION(Server, Reliable)
	void Server_EquipSlotClicked(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnEquip);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_EquipSlotClicked(UInv_InventoryItem* ItemToEquip, UInv_InventoryItem* ItemToUnEquip);
	
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ToggleInventoryMenu();

	void AddRepSubObject(UObject* SubObject);
	void SpawnDroppedItem(UInv_InventoryItem* Item, int32 StackCount);

	UInv_InventoryBase* GetInventoryMenu() const { return InventoryMenu; }

	FInventoryItemChange OnItemAdded;
	FInventoryItemChange OnItemRemoved;
	FNoRoomInInventory NoRoomInInventory;
	FStackChange OnStackChange;
	FItemEquipStatusChanged OnItemEquipped;
	FItemEquipStatusChanged OnItemUnequipped;
	FInventoryMenuToggled OnInventoryMenuToggled;

protected:
	virtual void BeginPlay() override;

private:
	void ConstructInventory();

	UPROPERTY(Replicated)
	FInv_InventoryFastArray InventoryList;

	TWeakObjectPtr<APlayerController> OwningController;

	UPROPERTY()
	TObjectPtr<class UInv_InventoryBase> InventoryMenu;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<class UInv_InventoryBase> InventoryMenuClass;

	bool bInventoryMenuOpen;
	void OpenInventoryMenu();
	void CloseInventoryMenu();


	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnAngleMin = -85.f;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnAngleMax = 85.f;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnDistanceMin = 50.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float DropSpawnDistanceMax = 150.f;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float RelativeSpawnElevation = 70.f;
};
