// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Widgets/Composite/Inv_CompositeBase.h"

void UInv_CompositeBase::Collapse()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UInv_CompositeBase::Expand()
{
	SetVisibility(ESlateVisibility::Visible);
}
