#include "Items/Fragments/Base/Inv_ItemFragment.h"
#include "Widgets/Composite/Inv_CompositeBase.h"


void FInv_InventoryItemFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	Composite->Expand();
}

bool FInv_InventoryItemFragment::MatchesWidgetTag(const UInv_CompositeBase* Composite) const
{
	return HasFragmentTag(Composite->GetFragmentTag());
}






