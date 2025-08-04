// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inv_Leaf.h"
#include "Inv_Leaf_ScrollableLabeledValues.generated.h"

class UTextBlock;
struct FInv_AbilitySystemModifier;
class UInv_Leaf_LabeledValue;
class UScrollBox;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_Leaf_ScrollableLabeledValues : public UInv_Leaf
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;

	UScrollBox* GetScrollBox() const {return ScrollBox;}

	void SetFontSizeLabel(const int32 Fontsize);
	void SetFontSizeValue(const int32 Fontsize);


private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox;

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
