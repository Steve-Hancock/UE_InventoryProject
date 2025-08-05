#include "Items/Fragments/Core/Inv_MultipleLabeledValueFragment.h"
#include "Items/Fragments/Core/Inv_LabeledValueFragment.h"


void FInv_MultipleLabeledValueFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();

	for (FInv_LabeledValueFragment& Fragment : LabeledValues)
	{
		Fragment.Manifest();
	}
}

void FInv_MultipleLabeledValueFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
}

FInv_LabeledValueFragment* FInv_MultipleLabeledValueFragment::GetLabeledValue(const int32 Index)
{
	if (LabeledValues.IsValidIndex(Index))
	{
		return &LabeledValues[Index];
	}
	return nullptr;
}
