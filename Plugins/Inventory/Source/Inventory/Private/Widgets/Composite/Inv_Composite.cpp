// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Widgets/Composite/Inv_Composite.h"
#include "Blueprint/WidgetTree.h"

void UInv_Composite::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	WidgetTree->ForEachWidget([this](UWidget* Widget)
	{
		UInv_CompositeBase* Composite = Cast<UInv_CompositeBase>(Widget);
		if (IsValid(Composite))
		{
			Children.Add(Composite);
			Composite->Collapse();
		}
	});
}

void UInv_Composite::ApplyFunction(FuncType Function)
{
	Super::ApplyFunction(Function);
	for (UInv_CompositeBase* Child : Children)
	{
		Child->ApplyFunction(Function);
	}
}

void UInv_Composite::Collapse()
{
	Super::Collapse();
	for (UInv_CompositeBase* Child : Children)
	{
		Child->Collapse();
	}
}
