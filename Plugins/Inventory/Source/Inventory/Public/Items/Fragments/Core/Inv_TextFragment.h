#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/Base/Inv_ItemFragment.h"
#include "Inv_TextFragment.generated.h"


USTRUCT(BlueprintType)
struct FInv_TextFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
public:
	FText GetText() const { return FragmentText; }
	void SetText(const FText& NewText) { FragmentText = NewText; }
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText FragmentText;
};