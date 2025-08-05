#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/Base/Inv_ItemFragment.h"
#include "Inv_EquippableFragment.generated.h"

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

	virtual FInv_AbilitySystemModifierFragment* FindModifer(int32 Index);
	virtual FInv_AbilitySystemModifierFragment* FindModiferByTag(const FGameplayTag& Tag);

	void OnEquip(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);
	void OnUnequip(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInv_AbilitySystemModifierFragment> AbilityModifiers;
};