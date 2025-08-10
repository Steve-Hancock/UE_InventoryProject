// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inv_EquipmentComponent.generated.h"

struct FGameplayTag;
struct FInv_ItemManifest;
struct FInv_EquippableFragment;
class AInv_EquipActor;
class UInv_InventoryItem;
class UInv_InventoryComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UInv_EquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:


protected:
	virtual void BeginPlay() override;

private:

	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;
	TWeakObjectPtr<APlayerController> OwningPlayerController;
	TWeakObjectPtr<USkeletalMeshComponent> OwningSkeletalMeshComponent;

	UFUNCTION()
	void OnItemEquipped(UInv_InventoryItem* EquippedItem);

	UFUNCTION()
	void OnItemUnequipped(UInv_InventoryItem* UnEquippedItem);

	void InitInventoryComponent();
	AInv_EquipActor* SpawnEquippedActor(FInv_EquippableFragment* EqquippableFragment,
		const FInv_ItemManifest& Manifest,
		USkeletalMeshComponent* AttachMesh );

	UPROPERTY()
	TArray<TObjectPtr<AInv_EquipActor>> EquippedActors;

	AInv_EquipActor* FindEquippedActorByTag(const FGameplayTag& EquipmentTypeTag);
	void RemoveEquippedActorByTag(const FGameplayTag& EquipmentTypeTag);
};
