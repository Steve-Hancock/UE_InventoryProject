#include "Items/Fragments/AbilitySystem/Inv_AbilitySystemModifierFragment.h"

void FInv_AbilitySystemModifierFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
}

void FInv_AbilitySystemModifierFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
}

