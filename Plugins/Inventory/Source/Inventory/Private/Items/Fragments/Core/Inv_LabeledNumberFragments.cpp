#include "Items/Fragments/Core/Inv_LabeledValueFragment.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"


void FInv_LabeledValueFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
	
	if (bRandomizeOnManifest)
	{
		Value = FMath::FRandRange(Min, Max);
	}
	
	bRandomizeOnManifest = false;
}

void FInv_LabeledValueFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);

	if (!MatchesWidgetTag(Composite)) return;
	UInv_Leaf_LabeledValue* LabeledValue = Cast<UInv_Leaf_LabeledValue>(Composite);
	if (!IsValid(LabeledValue)) return;

	FText FinalLabelText = GetLabelText();
	if (!bCollapseLabel && !LabelText.IsEmpty())
	{
		FString LabelString = LabelText.ToString();
		{
			if (!LabelString.EndsWith(":"))
			{
				LabelString += ":";
			}
			FinalLabelText = FText::FromString(LabelString);
		}
	}
	LabeledValue->SetTextLabel(FinalLabelText, bCollapseLabel);

	FNumberFormattingOptions NumberFormattingOptions;
	NumberFormattingOptions.MinimumFractionalDigits = MinFractionalDigits;
	NumberFormattingOptions.MaximumFractionalDigits = MaxFractionalDigits;

	FText FormattedText;
	FString Sign = Value < 0 ? "-" : "+";
	FString ValueString = FText::AsNumber(FMath::Abs(Value), &NumberFormattingOptions).ToString();

	if (bIsPercent)
	{
		FormattedText = FText::FromString(Sign + ValueString + TEXT("%"));
	}
	else
	{
		FormattedText = FText::FromString(Sign + ValueString);
	}
	LabeledValue->SetTextValue(FormattedText, bCollapseValue);
}