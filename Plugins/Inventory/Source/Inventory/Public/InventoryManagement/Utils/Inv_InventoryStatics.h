// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Types/Inv_GridTypes.h"
#include "Widgets/Utils/Inv_WidgetUtils.h"
#include "Inv_InventoryStatics.generated.h"

/**
 * 
 */

class UInv_HoverItem;
class UAbilitySystemComponent;
class UGameplayEffect;
struct FGameplayAbilitySpecHandle;
struct FActiveGameplayEffectHandle;


// Applied Systems Data
USTRUCT(BlueprintType)
struct FAppliedSystemsData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FActiveGameplayEffectHandle>	EffectHandles;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FGameplayAbilitySpecHandle>	AbilityHandles;
};

 
UCLASS()
class INVENTORY_API UInv_InventoryStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UInv_InventoryComponent* GetInventoryComponent(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static EInv_ItemCategory GetItemCategoryFromItemComponent(UInv_ItemComponent* ItemComponent);

	template<typename T, typename FuncT>
	static void ForEach2D(TArray<T>& Array, int32 Index, const FIntPoint& Range2D, int32 GridColumns, const FuncT& Function );

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static void ItemHovered(APlayerController* PlayerController, UInv_InventoryItem* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static void ItemUnHovered(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UInv_HoverItem* GetHoverItem(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UInv_InventoryBase* GetInventoryWidget(APlayerController* PlayerController);


	// Apply Ability System Data
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static UAbilitySystemComponent* GetAbilitySystemComponent(APlayerController* PlayerController);
};

template <typename T, typename FuncT>
void UInv_InventoryStatics::ForEach2D(TArray<T>& Array, int32 Index, const FIntPoint& Range2D, int32 GridColumns,
	const FuncT& Function)
{
	for ( int32 j = 0; j < Range2D.Y; ++j )
	{
		for ( int32 i = 0; i < Range2D.X; ++i )
		{
			const FIntPoint Coordinates =  UInv_WidgetUtils::GetPositionFromIndex(Index, GridColumns) + FIntPoint(i, j);
			const int32 TileIndex = UInv_WidgetUtils::GetIndexFromPosition(Coordinates, GridColumns);
			if (Array.IsValidIndex(TileIndex))
			{
				Function(Array[TileIndex]);
			}
		}
	}
}
