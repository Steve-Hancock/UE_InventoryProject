// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Widgets/Composite/Inv_Leaf_MultipleLabeledValues.h"

#include "Components/VerticalBox.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"

void UInv_Leaf_MultipleLabeledValues::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInv_Leaf_MultipleLabeledValues::SetFontSizeLabel(const int32 Fontsize)
{
	FontSizeLabel = Fontsize;
	if (!VerticalBox) return;

	for ( UWidget* Child : VerticalBox->GetAllChildren())
	{
		UInv_Leaf_LabeledValue* LabeledValue = Cast<UInv_Leaf_LabeledValue>(Child);
		if (LabeledValue)
		{
			LabeledValue->SetFontSizeLabel(FontSizeLabel);
		}
	}
}

void UInv_Leaf_MultipleLabeledValues::SetFontSizeValue(const int32 Fontsize)
{
	FontSizeValue = Fontsize;
	if (!VerticalBox) return;
	
	for ( UWidget* Child : VerticalBox->GetAllChildren())
	{
		UInv_Leaf_LabeledValue* LabeledValue = Cast<UInv_Leaf_LabeledValue>(Child);
		if (LabeledValue)
		{
			LabeledValue->SetFontSizeValue(FontSizeValue);
		}
	}
}
