// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Interaction/Inv_HighlightableStaticMesh.h"

void UInv_HighlightableStaticMesh::Highlight_Implementation()
{
	IInv_Highlightable::Highlight_Implementation();
	SetOverlayMaterial(HighlightedMaterial);
}

void UInv_HighlightableStaticMesh::UnHighlight_Implementation()
{
	IInv_Highlightable::UnHighlight_Implementation();
	SetOverlayMaterial(nullptr);
}
