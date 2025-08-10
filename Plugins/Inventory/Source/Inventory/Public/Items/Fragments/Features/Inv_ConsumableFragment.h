#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/Base/Inv_ItemFragment.h"
#include "StructUtils/InstancedStruct.h"
#include "Inv_ConsumableFragment.generated.h"


struct FInv_AbilityModifierFragment;
struct FInv_EffectModifierFragment;

// Consumable Fragment, used to add a consumable effect to an item.
USTRUCT(BlueprintType)
struct FInv_ConsumableFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	
	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	
	void OnConsume(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<TInstancedStruct<FInv_EffectModifierFragment>>  EffectModifiers;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<TInstancedStruct<FInv_AbilityModifierFragment>>  Abilities;
};