// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "Widgets/Utils/Inv_WidgetUtils.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/Widget.h"

int32 UInv_WidgetUtils::GetIndexFromPosition(const FIntPoint& Position, const int32 Columns)
{
	return Position.X + Position.Y * Columns;
}

FIntPoint UInv_WidgetUtils::GetPositionFromIndex(const int32 Index, const int32 Columns)
{
	return FIntPoint(Index % Columns, Index / Columns);
}

FVector2D UInv_WidgetUtils::GetWidgetPosition(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	
	FVector2D PixelPosition {};
	FVector2D ViewportPosition {};
	USlateBlueprintLibrary::LocalToViewport(Widget, Geometry, USlateBlueprintLibrary::GetLocalTopLeft(Geometry), PixelPosition, ViewportPosition );
	return ViewportPosition;
}

bool UInv_WidgetUtils::IsWithinBounds(const FVector2D& BoundaryPosition, const FVector2D& WidgetSize,
	const FVector2D& MousePosition)
{
	return MousePosition.X >= BoundaryPosition.X && MousePosition.X <= (BoundaryPosition.X + WidgetSize.X) &&
		MousePosition.Y >= BoundaryPosition.Y && MousePosition.Y <= (BoundaryPosition.Y + WidgetSize.Y);
}

FVector2D UInv_WidgetUtils::GetWidgetSize(UWidget* Widget)
{
	const FGeometry Geometry = Widget->GetCachedGeometry();
	return Geometry.GetLocalSize();
}

FVector2D UInv_WidgetUtils::GetClampedWidgetPosition(const FVector2D& BoundaryPosition, const FVector2D& WidgetSize,
	const FVector2D& MousePosition)
{
	FVector2D ClampedPosition = MousePosition;

	// Adjust the horizontal position to ensure that the widget stays within the boundary.
	if ( MousePosition.X + WidgetSize.X > BoundaryPosition.X ) // Widget exceds the right edge
	{
		ClampedPosition.X = BoundaryPosition.X - WidgetSize.X;
	}

	if ( MousePosition.X < 0) // Widget exceeds the left edge
	{
		ClampedPosition.X = 0;
	}

	// Adjust the vertical position to ensure that the widget stays within the boundary
	if ( MousePosition.Y + WidgetSize.Y > BoundaryPosition.Y ) // Widget exceeds the bottom edge
	{
		ClampedPosition.Y = BoundaryPosition.Y - WidgetSize.Y;
	}

	if ( MousePosition.Y < 0 ) // Widget exceeds the top edge
	{
		ClampedPosition.Y = 0;
	}
	
	return ClampedPosition;
}
