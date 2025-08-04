// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Inv_Leaf.h"
#include "Inv_Leaf_Image.generated.h"

class USizeBox;
class UImage;
/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_Leaf_Image : public UInv_Leaf
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetImage(UTexture2D* Texture) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetBoxSize(const FVector2D& Size) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetImageSize(const FVector2D& Size) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FVector2D GetBoxSize() const;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Icon;
};
