#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/Base/Inv_ItemFragment.h"
#include "Inv_LabeledValueFragment.h"
#include "Inv_MultipleLabeledValueFragment.generated.h"


USTRUCT(Blueprintable)
struct FInv_MultipleLabeledValueFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

	TArray<FInv_LabeledValueFragment> GetLabeledValues() const { return LabeledValues; }
	TArray<FInv_LabeledValueFragment>& GetLabeledValuesMutable() { return LabeledValues; }
	FInv_LabeledValueFragment* GetLabeledValue(const int32 Index); 

private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInv_LabeledValueFragment> LabeledValues;
	
};