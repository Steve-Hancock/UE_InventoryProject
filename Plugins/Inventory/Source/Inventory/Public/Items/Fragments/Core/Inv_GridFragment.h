#pragma once

#include "Items/Fragments/Base/Inv_ItemFragment.h"
#include "Inv_GridFragment.generated.h"


// Grid Fragment, use to determine grid size and its padding, example 1x1, 2x2 4x2 ext...
USTRUCT(BlueprintType)
struct FInv_GridFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

public:

	FIntPoint GetGridSize() const { return GridSize; }
	float GetGridPadding() const { return GridPadding; }

	void SetGridSize(const FIntPoint& NewSize) { GridSize = NewSize; }
	void SetGridPadding(const float NewPadding) { GridPadding = NewPadding; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{1,1};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding{0.0f};
};