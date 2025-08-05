// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inv_Leaf.h"
#include "Inv_Leaf_MultipleLabeledValues.generated.h"

class UVerticalBox;
class UTextBlock;
class UInv_Leaf_LabeledValue;

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_Leaf_MultipleLabeledValues : public UInv_Leaf
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;

	UVerticalBox* GetVerticalBox() const {return VerticalBox;}

	void SetFontSizeLabel(const int32 Fontsize);
	void SetFontSizeValue(const int32 Fontsize);


private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_Leaf_LabeledValue> LabeledValueClass;
	
	UPROPERTY(meta = (BindWidget))
	TArray<TObjectPtr<UTextBlock>> Labels;

	UPROPERTY(meta = (BindWidget))
	TArray<TObjectPtr<UTextBlock>> Values;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSizeLabel{18};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSizeValue{18};
	
};
