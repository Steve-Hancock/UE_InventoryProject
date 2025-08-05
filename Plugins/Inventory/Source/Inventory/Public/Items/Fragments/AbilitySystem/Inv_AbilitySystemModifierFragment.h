#pragma once

#include "CoreMinimal.h"
#include "Inv_AbilitySystemData.h"
#include "Items/Fragments/Base/Inv_ItemFragment.h"
#include "Inv_AbilitySystemModifierFragment.generated.h"

USTRUCT(BlueprintType)
struct FAttributeDisplayOptions
{
	GENERATED_BODY()

	// Custom display name (Empty = use attribute name)
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText DisplayName;

	// Show +/- prefix for positive/negative values
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bShowSign = true;

	// Show percentage sign for percentage values
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bShowPercentage = false;

	FAttributeDisplayOptions()
	{
		DisplayName = FText::GetEmpty();
		bShowSign = true;
		bShowPercentage = false;
	}
};


USTRUCT(BlueprintType)
struct FInv_AbilitySystemModifierFragment : public FInv_InventoryItemFragment

{
	GENERATED_BODY()

public:
	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

	void ApplyModifer(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);
	void RemoveModifer(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);
	
	TSubclassOf<UInv_Leaf_LabeledValue> GetLabeledValueClass() const { return LabeledValueClass; }
	FInv_AbilitySystemData GetAbilityData() const { return AbilityData; }

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FInv_AbilitySystemData AbilityData;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_Leaf_LabeledValue> LabeledValueClass;
	
	//Map to configure display options for each attribute
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ToolTip = "Configure display options for attributes. Key = Attribute Name"))
	TMap<FString, FAttributeDisplayOptions> AttributeDisplayOptions;

	FText FormatAttributeValue(float Value, const FAttributeDisplayOptions& Options) const;

};
