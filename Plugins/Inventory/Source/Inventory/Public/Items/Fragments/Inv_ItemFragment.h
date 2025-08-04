#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"


#include "Inv_ItemFragment.generated.h"

class UInv_Leaf_LabeledValue;
struct FInv_AbilitySystemModifier;
struct FAppliedSystemsData;
class UGameplayAbility;
class UInv_CompositeBase;
class UInv_InventoryItem;
class APlayerController;
class UGameplayEffect;
class UGameplayCueNotify_Static;



#pragma region Base Struct
// Item Fragment, The Base class, not used for anything alone.

USTRUCT(BlueprintType)
struct FInv_ItemFragment
{
	GENERATED_BODY()

	FInv_ItemFragment() {};
	FInv_ItemFragment(const FInv_ItemFragment& Other) = default;
	FInv_ItemFragment& operator=(const FInv_ItemFragment& Other) = default;
	FInv_ItemFragment(FInv_ItemFragment&& Other) = default;
	FInv_ItemFragment& operator=(FInv_ItemFragment&& Other) = default;
	virtual ~FInv_ItemFragment() {}

	FGameplayTagContainer GetFragmentTagContainer() const { return FragmentTags; }
	void AddFragmentTag(const FGameplayTag& Tag) { FragmentTags.AddTag(Tag); }
	void RemoveFragmentTag(const FGameplayTag& Tag) { FragmentTags.RemoveTag(Tag); }
	bool HasFragmentTag(const FGameplayTag& Tag) const { return FragmentTags.HasTagExact(Tag); }
	bool DoesHaveAbilityModifiers() const { return bHasAbilityModifiers; }

	//virtual function that fragments with modifiers can override
	virtual FInv_AbilitySystemModifier* FindModiferByTag(const FGameplayTag& Tag)  {return nullptr;}
	virtual FInv_AbilitySystemModifier* FindModifer(const int32 Index)  {return nullptr;}

	
	virtual void Manifest() {}

protected:
	// Set to true in fragments that contain AbilityModifiers arrays
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bHasAbilityModifiers{false};
	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta =(Categories="FragmentTags"))
	FGameplayTagContainer FragmentTags{};
};
#pragma endregion

#pragma region Inventory Item Fragment

/*
 *	Item fragment specifically for assimilation into a widget.
 *	Not used alone, but to be extended with child fragments.
 *	
 */
USTRUCT(BlueprintType)
struct FInv_InventoryItemFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(UInv_CompositeBase* Composite) const;

protected:
	bool MatchesWidgetTag(const UInv_CompositeBase* Composite) const;
};

#pragma endregion

#pragma region Grid Fragment Structs
// Grid Fragment, use to determine grid size and its padding, example 1x1, 2x2 4x2 ext...
USTRUCT(BlueprintType)
struct FInv_GridFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

public:

	FIntPoint GetGridSize() const { return GridSize; }
	float GetGridPadding() const { return GridPadding; }

	void SetGridSize(const FIntPoint& NewSize) { GridSize = NewSize; }
	void SetGridPadding(const float NewPadding) { GridPadding = NewPadding; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{1,1};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding{0.0f};
	
};

#pragma endregion

#pragma region Image Fragment
// Image Fragment, used to add an Icon or image to an item.
USTRUCT(BlueprintType)
struct FInv_ImageFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
public:
	UTexture2D* GetIcon() const { return Icon; }
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	FVector2D GetIconDimensions() const { return IconDimensions; }
	void SetIcon(UTexture2D* NewIcon) { Icon = NewIcon; }
	void SetIconDimensions(const FVector2D& NewDimensions) { IconDimensions = NewDimensions; }

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	UTexture2D* Icon{nullptr};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{44.0f, 44.0f};
};
#pragma endregion

#pragma region Stackable Fragment
// Stackable Fragment, used to stack items that can be stacked in inventory
USTRUCT(BlueprintType)
struct FInv_StackableFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	int32 GetMaxStackSize() const { return MaxStackSize; }
	int32 GetStackCount() const { return StackCount; }
	
	void SetMaxStackSize(const int32 NewSize) { MaxStackSize = NewSize; }
	void SetStackCount(const int32 NewCount) { StackCount = NewCount; }

	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxStackSize{1};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 StackCount{1};
};

#pragma endregion

#pragma region Text Fragment
// Text Fragment Used for displaying text info on item information display, or anyplace you want to display text.
USTRUCT(BlueprintType)
struct FInv_TextFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
public:
	FText GetText() const { return FragmentText; }
	void SetText(const FText& NewText) { FragmentText = NewText; }
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText FragmentText;
};

#pragma endregion

#pragma region Labeled Number Fragment
// Labeled Number Fragment, generates a text label with a random float value.
USTRUCT(BlueprintType)
struct FInv_LabeledNumberFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

	// When manifesting for the first time, this fragment will randomize.  However, once
	// the item has been picked up, the value will not change even if dropped and picked up again.
	bool bRandomizeOnManifest{true};

	// Getters for private members
	FText GetLabelText() const { return LabelText; }
	bool IsCollapseLabel() const { return bCollapseLabel; }
	bool IsCollapseValue() const { return bCollapseValue; }
	int32 GetMinFractionalDigits() const { return MinFractionalDigits; }
	int32 GetMaxFractionalDigits() const { return MaxFractionalDigits; }
	float GetValue() const { return Value; }
	bool IsPercent() const { return bIsPercent; }

	// Setters for private members
	void SetLabelText(const FText& NewLabel) { LabelText = NewLabel; }
	void SetCollapseLabel(const bool NewCollapse) { bCollapseLabel = NewCollapse; }
	void SetCollapseValue(const bool NewCollapse) { bCollapseValue = NewCollapse; }
	void SetMinFractionalDigits(const int32 NewDigits) { MinFractionalDigits = NewDigits; }
	void SetMaxFractionalDigits(const int32 NewDigits) { MaxFractionalDigits = NewDigits; }
	void SetValue(const float NewValue) { Value = NewValue; }

	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText LabelText{};

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float Value{0.f};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Min{0};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Max{0};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseLabel{false};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseValue{false};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bIsPercent{false};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MinFractionalDigits{1};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxFractionalDigits{1};
};


#pragma endregion

#pragma region Scrollable Labeled Number Fragment
USTRUCT(BlueprintType)
struct FInv_ScrollableLabelNumberFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()

	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

	TArray<FText> GetLabels() const { return Labels; }
	FText GetLabelText(const int32 Index) const { return Labels[Index]; }
	FText GetValueText(const int32 Index) const { return Values[Index]; }
	int32 GetFontSizeLabels() const { return FontSizeLabels; }
	int32 GetFontSizeValues() const { return FontSizeValues; }
	int32 GetMinFractionalDigits() const { return MinFractionalDigits; }
	int32 GetMaxFractionalDigits() const { return MaxFractionalDigits; }
	TArray<FText> GetValues() const { return Values; }
	TSubclassOf<UInv_Leaf_LabeledValue> GetLabeledValueClass() const { return LabeledValueClass; }

private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FText> Labels;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FText> Values;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_Leaf_LabeledValue> LabeledValueClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSizeLabels {18};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSizeValues {18};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MinFractionalDigits{1};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxFractionalDigits{1};
	
};


#pragma endregion

#pragma region AbilitySystem Shared Data
USTRUCT(BlueprintType)
struct FInv_AbilitySystemData
{
	GENERATED_BODY()

	// Effects 
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EffectTag;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float EffectLevel = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EffectCueTag;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayCueNotify_Static> EffectCueClass;

	// Abilities
	UPROPERTY(editAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayAbility> AbilityToGrant;

	UPROPERTY(editAnywhere, Category = "Inventory")
	FGameplayTag AbilityTag;

	UPROPERTY(editAnywhere, Category = "Inventory")
	float AbilityLevel = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool GrantAbilityPermanently = false;

	// Applies only to permanent abilities. Non-permanent abilities are always activated immediately.
	// If this is true, the permanent ability will also be activated once when it is granted (e.g., for passive or auto-triggered effects).
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool ActivateAbilityImmediately = false;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag AbilityCueTag;
	
	UPROPERTY(editAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayCueNotify_Static> AbilityCueClass;

	UPROPERTY()
	FActiveGameplayEffectHandle ActiveEffectHandle;;

	UPROPERTY()
	FGameplayAbilitySpecHandle ActiveAbilityHandle;
	
	UPROPERTY()
	TMap<FGameplayTag, float> SetByCallerMagnitudes;
	
	FGameplayAbilitySpecHandle ApplyAbilityAndAbilityCue(APlayerController* PlayerController);
	FGameplayAbilitySpecHandle ApplyAbilityAndAbilityCuePermanently(APlayerController* PlayerController);
	FActiveGameplayEffectHandle ApplyEffectAndEffectCue(APlayerController* PlayerController);


	float GetEffectLevel() const {return EffectLevel;}
	float GetAbilityLevel() const {return AbilityLevel;}

	void SetEffectLevel(float NewLevel) {EffectLevel = NewLevel;}
	void SetAbilityLevel(float NewLevel) {AbilityLevel = NewLevel;}
	
	
	void RemoveEffect(APlayerController* PlayerController);
	void RemoveAbility(APlayerController* PlayerController);
	
	auto GetEffectMagnitude(int32 Index) const -> float;
};

#pragma endregion

#pragma region Ability System Modifier
USTRUCT(BlueprintType)
struct FInv_AbilitySystemModifier : public FInv_ScrollableLabelNumberFragment
{
	GENERATED_BODY()
	
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	virtual void Manifest() override;
	
	void ApplyModifier(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);
	void RemoveModifier(APlayerController* PlayerController,
		UInv_InventoryItem* Item = nullptr);
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FInv_AbilitySystemData AbilitySystemData;

	// This array should match the number of modifiers used.
	// If the array is empty or the index is out of range,
	// the UI will default to displaying without a percent sign.
	// If the value at a given index is true, the corresponding modifier’s value
	// will display with a "%" symbol in the UI.
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<bool> UsePercent;

};

#pragma endregion

#pragma region Consumable Fragment
// Consumable Fragment, used to add a consumable effect to an item.
USTRUCT(BlueprintType)
struct FInv_ConsumableFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	
	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	virtual FInv_AbilitySystemModifier* FindModifer(int32 Index)  override;
	virtual FInv_AbilitySystemModifier* FindModiferByTag(const FGameplayTag& Tag)  override;

	
	void OnConsume(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInv_AbilitySystemModifier> AbilityModifiers;
};

#pragma endregion

#pragma region Equippable Fragment
// Equippable Fragment, used to add an equippable effect to an item.
USTRUCT(BlueprintType)
struct FInv_EquippableFragment : public FInv_InventoryItemFragment
{
	GENERATED_BODY()
	
	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;

	virtual FInv_AbilitySystemModifier* FindModifer(int32 Index)  override;
	virtual FInv_AbilitySystemModifier* FindModiferByTag(const FGameplayTag& Tag)  override;

	void OnEquip(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);
	void OnUnequip(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TArray<FInv_AbilitySystemModifier> AbilityModifiers;
};

#pragma endregion

/*
 * Todo: DurabilityFragments
 * Todo: QualityFragments
 * Todo: RarityFragments
 * Todo: WeightFragments
 * Todo: ItemValueFragments
 * Todo: ItemSocketFragments
 * Todo: OnOverlapFragments
 * Todo: QuestItemFragment
 * Todo: AppearanceFragment
 * Todo: CraftableItemsFragment
 * Todo: CollectionItemsFragment
 * Todo: BindableItemFragment
 * Todo: ItemLevelingFragment // Items that gane xp and level with the character
 * Todo: ItemUpgradeFragment
 * Todo: ItemEnchantmentFragment
 */
