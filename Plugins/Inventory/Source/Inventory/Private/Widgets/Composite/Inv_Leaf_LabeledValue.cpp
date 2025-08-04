// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"

#include "Components/TextBlock.h"

void UInv_Leaf_LabeledValue::NativePreConstruct()
{
	Super::NativePreConstruct();

	FSlateFontInfo FontInfoLabel = TextLabel->GetFont();
	FontInfoLabel.Size = FontSizeLabel;
	TextLabel->SetFont(FontInfoLabel);

	FSlateFontInfo FontInfoValue = TextValue->GetFont();
	FontInfoValue.Size = FontSizeValue;
	TextValue->SetFont(FontInfoValue);
}

void UInv_Leaf_LabeledValue::SetTextLabel(const FText& Label, bool bCollapse) const
{
	if (bCollapse)
	{
		TextLabel->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	TextLabel->SetText(Label);
}

void UInv_Leaf_LabeledValue::SetTextValue(const FText& Value, bool bCollapse) const
{
	if (bCollapse)
	{
		TextValue->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	TextValue->SetText(Value);
}

void UInv_Leaf_LabeledValue::SetFontSizeLabel(const int32 Fontsize)
{
	FontSizeLabel = Fontsize;

	if (TextLabel)
	{
		FSlateFontInfo FontInfo = TextLabel->GetFont();
		FontInfo.Size = FontSizeLabel;
		TextLabel->SetFont(FontInfo);
	}
}

void UInv_Leaf_LabeledValue::SetFontSizeValue(const int32 Fontsize)
{
	FontSizeValue = Fontsize;
	
	if (TextValue)
	{
		FSlateFontInfo FontInfo = TextValue->GetFont();
		FontInfo.Size = FontSizeValue;
		TextValue->SetFont(FontInfo);
	}
}


