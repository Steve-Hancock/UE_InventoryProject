// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Widgets/Composite/Inv_Leaf.h"

void UInv_Leaf::ApplyFunction(FuncType Function)
{
	Function(this);
}
