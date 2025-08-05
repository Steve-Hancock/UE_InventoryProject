#pragma once

#include "CoreMinimal.h"
#include "Items/Fragments/Base/Inv_ItemFragment.h"
#include "Inv_ImageFragment.generated.h"

// Image Fragment, used to add an Icon or image to an item.
USTRUCT(BlueprintType)
struct FInv_ImageFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
public:
	UTexture2D* GetIcon() const { return Icon; }
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	FVector2D GetIconDimensions() const { return IconDimensions; }
	void SetIcon(UTexture2D* NewIcon) { Icon = NewIcon; }
	void SetIconDimensions(const FVector2D& NewDimensions) { IconDimensions = NewDimensions; }

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	UTexture2D* Icon{nullptr};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{44.0f, 44.0f};
};