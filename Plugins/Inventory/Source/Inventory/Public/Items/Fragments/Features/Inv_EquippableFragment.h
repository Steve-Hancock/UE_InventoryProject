#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/Base/Inv_ItemFragment.h"
#include "StructUtils/InstancedStruct.h"
#include "Inv_EquippableFragment.generated.h"

class AInv_EquipActor;
struct FInv_AbilityModifierFragment;
struct FInv_EffectModifierFragment;
struct FInv_AbilitySystemModifierFragment;
class APlayerController;
class UInv_CompositeBase;

// Equippable Fragment, used to add an equippable effect to an item.
USTRUCT(BlueprintType)
struct FInv_EquippableFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	
	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	
	void OnEquip(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);
	void OnUnequip(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);

	AInv_EquipActor* SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const;
	void DestroyAttachedActor();

	FGameplayTag GetEquipmentType() const { return EquipmentType; }

	void SetEquippedActor(AInv_EquipActor* EquipActor);

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<TInstancedStruct<FInv_EffectModifierFragment>>  EffectModifiers;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<TInstancedStruct<FInv_AbilityModifierFragment>>  Abilities;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<AInv_EquipActor> EquipActorClass = nullptr;

	TWeakObjectPtr<AInv_EquipActor> EquippedActor;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FName SocketAttachPoint {NAME_None};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EquipmentType = FGameplayTag::EmptyTag;
};