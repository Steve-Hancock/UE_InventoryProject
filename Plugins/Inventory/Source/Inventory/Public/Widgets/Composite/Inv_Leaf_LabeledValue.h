// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inv_Leaf.h"
#include "Inv_Leaf_LabeledValue.generated.h"

class USizeBox;
class UTextBlock;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_Leaf_LabeledValue : public UInv_Leaf
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	
	void SetTextLabel(const FText& Label, bool bCollapse) const;
	void SetTextValue(const FText& Value, bool bCollapse) const;
	void SetFontSizeLabel(const int32 Fontsize);
	void SetFontSizeValue(const int32 Fontsize);

	

private:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextLabel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextValue;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSizeLabel{18};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSizeValue{18};
};
