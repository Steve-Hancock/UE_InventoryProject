#pragma once		
#include "Items/Fragments/Base/Inv_ItemFragment.h"
#include "Inv_StackableFragment.generated.h"


// Stackable Fragment, used to stack items that can be stacked in inventory
USTRUCT(BlueprintType)
struct FInv_StackableFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	int32 GetMaxStackSize() const { return MaxStackSize; }
	int32 GetStackCount() const { return StackCount; }
	
	void SetMaxStackSize(const int32 NewSize) { MaxStackSize = NewSize; }
	void SetStackCount(const int32 NewCount) { StackCount = NewCount; }

	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxStackSize{1};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 StackCount{1};
};