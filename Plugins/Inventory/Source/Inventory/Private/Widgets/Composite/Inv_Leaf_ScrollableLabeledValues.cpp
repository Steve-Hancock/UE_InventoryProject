// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Widgets/Composite/Inv_Leaf_ScrollableLabeledValues.h"

#include "Components/ScrollBox.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"

void UInv_Leaf_ScrollableLabeledValues::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (ScrollBox)
	{
		ScrollBox->ClearChildren();
	}
}

void UInv_Leaf_ScrollableLabeledValues::SetFontSizeLabel(const int32 Fontsize)
{
	FontSizeLabel = Fontsize;
	if (!ScrollBox) return;

	for ( UWidget* Child : ScrollBox->GetAllChildren())
	{
		UInv_Leaf_LabeledValue* LabeledValue = Cast<UInv_Leaf_LabeledValue>(Child);
		if (LabeledValue)
		{
			LabeledValue->SetFontSizeLabel(FontSizeLabel);
		}
	}
}

void UInv_Leaf_ScrollableLabeledValues::SetFontSizeValue(const int32 Fontsize)
{
	FontSizeValue = Fontsize;
	if (!ScrollBox) return;
	
	for ( UWidget* Child : ScrollBox->GetAllChildren())
	{
		UInv_Leaf_LabeledValue* LabeledValue = Cast<UInv_Leaf_LabeledValue>(Child);
		if (LabeledValue)
		{
			LabeledValue->SetFontSizeValue(FontSizeValue);
		}
	}
}
