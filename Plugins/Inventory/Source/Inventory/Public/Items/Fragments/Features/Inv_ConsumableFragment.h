#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/Base/Inv_ItemFragment.h"
#include "Inv_ConsumableFragment.generated.h"


struct FInv_AbilitySystemModifierFragment;

// Consumable Fragment, used to add a consumable effect to an item.
USTRUCT(BlueprintType)
struct FInv_ConsumableFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	
	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	virtual FInv_AbilitySystemModifierFragment* FindModifer(int32 Index);
	virtual FInv_AbilitySystemModifierFragment* FindModiferByTag(const FGameplayTag& Tag);

	
	void OnConsume(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInv_AbilitySystemModifierFragment> AbilityModifiers;
};