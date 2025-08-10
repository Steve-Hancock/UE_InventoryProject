#include "Items/Fragments/Features/Inv_ConsumableFragment.h"

#include "Items/Fragments/AbilitySystem/Inv_AbilityModifierFragment.h"
#include "Items/Fragments/AbilitySystem/Inv_AbilitySystemModifierFragment.h"
#include "Items/Fragments/AbilitySystem/Inv_EffectModifierFragment.h"

void FInv_ConsumableFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
}

void FInv_ConsumableFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;
	
	 for ( const TInstancedStruct<FInv_EffectModifierFragment>& Modifer : EffectModifiers)
	 {
		 if (Modifer.IsValid())
		 {
		 	const FInv_EffectModifierFragment& Modifier = Modifer.Get();
		 	Modifier.Assimilate(Composite);
		 }
	 }
	
}


void  FInv_ConsumableFragment::OnConsume(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if ( !IsValid(PlayerController) ) return;

	if (EffectModifiers.Num() > 0 )
	{
		for (TInstancedStruct<FInv_EffectModifierFragment>& Modifer : EffectModifiers)
		{
			if (Modifer.IsValid())
			{
				Modifer.GetMutable<FInv_EffectModifierFragment>().OnApply(PlayerController, Item);
			}

		}
	}

	if ( Abilities.Num() > 0)
	{
		for (TInstancedStruct<FInv_AbilityModifierFragment>& Ability : Abilities)
		{
			Ability.GetMutable<FInv_AbilityModifierFragment>().OnApply(PlayerController, Item);
		}
	}
}
