#pragma once

#include "CoreMinimal.h"
#include "items/Fragments/Base/Inv_ItemFragment.h"
#include "Inv_LabeledValueFragment.generated.h"

// Labeled Number Fragment, generates a text label with a random float value.
USTRUCT(BlueprintType)
struct FInv_LabeledValueFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

	// When manifesting for the first time, this fragment will randomize.  However, once
	// the item has been picked up, the value will not change even if dropped and picked up again.
	bool bRandomizeOnManifest{true};

	// Getters for private members
	FText GetLabelText() const { return LabelText; }
	bool IsCollapseLabel() const { return bCollapseLabel; }
	bool IsCollapseValue() const { return bCollapseValue; }
	int32 GetMinFractionalDigits() const { return MinFractionalDigits; }
	int32 GetMaxFractionalDigits() const { return MaxFractionalDigits; }
	float GetValue() const { return Value; }
	bool IsPercent() const { return bIsPercent; }

	// Setters for private members
	void SetLabelText(const FText& NewLabel) { LabelText = NewLabel; }
	void SetCollapseLabel(const bool NewCollapse) { bCollapseLabel = NewCollapse; }
	void SetCollapseValue(const bool NewCollapse) { bCollapseValue = NewCollapse; }
	void SetMinFractionalDigits(const int32 NewDigits) { MinFractionalDigits = NewDigits; }
	void SetMaxFractionalDigits(const int32 NewDigits) { MaxFractionalDigits = NewDigits; }
	void SetValue(const float NewValue) { Value = NewValue; }

	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText LabelText{};

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float Value{0.f};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Min{0};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Max{0};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseLabel{false};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseValue{false};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bIsPercent{false};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MinFractionalDigits{1};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxFractionalDigits{1};
};