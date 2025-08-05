#include "Items/Fragments/Features/Inv_ConsumableFragment.h"
#include "Items/Fragments/AbilitySystem/Inv_AbilitySystemModifierFragment.h"

void FInv_ConsumableFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
}

void FInv_ConsumableFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	
	 for ( const FInv_AbilitySystemModifierFragment& Modifer : AbilityModifiers)
	 {
	 	Modifer.Assimilate(Composite);
	 }

	FInv_InventoryItemFragment::Assimilate(Composite);
}

FInv_AbilitySystemModifierFragment* FInv_ConsumableFragment::FindModifer(int32 Index) 
{
	 if (AbilityModifiers.IsValidIndex(Index))
	 {
	 	return &AbilityModifiers[Index];
	 }

	return nullptr;
}

FInv_AbilitySystemModifierFragment* FInv_ConsumableFragment::FindModiferByTag(const FGameplayTag& Tag)
{
	 if (AbilityModifiers.Num() == 0) return nullptr;
	
	 for (auto& Data : AbilityModifiers )
	 {
		FInv_AbilitySystemModifierFragment* Modifer = &Data;
	 		if ( Data.GetAbilityData().EffectTag == Tag )
	 	{
	 			return Modifer;
	 	}
	
	 		if ( Data.GetAbilityData().AbilityTag == Tag )
	 	{
	 			return Modifer;
	 	}
	 }

	return nullptr;
}

void  FInv_ConsumableFragment::OnConsume(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;

	if (AbilityModifiers.Num() == 0) return;
	
	 for ( auto& Data : AbilityModifiers )
	 {
		Data.ApplyModifer(PlayerController, Item);
	 }
}
