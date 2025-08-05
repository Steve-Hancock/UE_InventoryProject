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
	
	virtual void Manifest() {}

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta =(Categories="FragmentTags"))
	FGameplayTagContainer FragmentTags{};
};


/*
 *	Item fragment specifically for assimilation into a widget.
 *	Not used alone, but to be extended with child fragments.
 */
USTRUCT(BlueprintType)
struct FInv_InventoryItemFragment : public FInv_ItemFragment
{
	GENERATED_BODY()

	virtual void Assimilate(UInv_CompositeBase* Composite) const;

protected:
	bool MatchesWidgetTag(const UInv_CompositeBase* Composite) const;
};



