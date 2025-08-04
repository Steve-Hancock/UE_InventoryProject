// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Widgets/Composite/Inv_Leaf_Text.h"

#include "Components/TextBlock.h"

void UInv_Leaf_Text::NativePreConstruct()
{
	Super::NativePreConstruct();
	FSlateFontInfo FontInfo = Text_LeafText->GetFont();
	FontInfo.Size = FontSize;
	Text_LeafText->SetFont(FontInfo);
	Text_LeafText->SetAutoWrapText(UseWrapText);
	if (UseWrapText)
	{
		Text_LeafText->SetWrapTextAt(Wrapwidth);
	}
}

void UInv_Leaf_Text::SetText(const FText& Text) const
{
	Text_LeafText->SetText(Text);
}
