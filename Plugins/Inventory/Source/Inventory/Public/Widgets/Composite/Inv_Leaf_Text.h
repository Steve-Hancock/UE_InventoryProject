// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inv_Leaf.h"
#include "Inv_Leaf_Text.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_Leaf_Text : public UInv_Leaf
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	void SetText(const FText& Text) const;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_LeafText;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSize{12};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Wrapwidth {50};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool UseWrapText {false};
};
