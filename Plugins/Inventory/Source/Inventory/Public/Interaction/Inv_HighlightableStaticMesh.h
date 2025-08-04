// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Inv_Highlightable.h"
#include "Inv_HighlightableStaticMesh.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_HighlightableStaticMesh : public UStaticMeshComponent, public IInv_Highlightable

{
	GENERATED_BODY()
public:
	// INV_Highlightable Interface
	virtual void Highlight_Implementation() override;
	virtual void UnHighlight_Implementation() override;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UMaterialInterface> HighlightedMaterial;
};
