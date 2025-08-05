#pragma once
#include "NativeGameplayTags.h"

namespace FragmentTags
{
	/*
	 * Base Function Fragment Tags
	 * These tags map directly a specific fragment struct
	 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GridFragment);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ImageFragment);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(StackableFragment);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ConsumableFragment);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquippableFragment);
	//UE_DECLARE_GAMEPLAY_TAG_EXTERN(CraftableFragment);
	//UE_DECLARE_GAMEPLAY_TAG_EXTERN(CollectableFragment);
	//UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemDurabilityFragment);
	//UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemQualityFragment);
	//UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemRarityFragment);
	//UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemWeightFragment);
	//UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemQuestFragment);

	/*
	* Tags use with FInv_TextFragment
	* These determine display text, but not structure
	*/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemNameFragment);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FlavorTextFragment);
	//UE_DECLARE_GAMEPLAY_TAG_EXTERN(RequiredLevelFragment);
	//UE_DECLARE_GAMEPLAY_TAG_EXTERN(ItemTypeFragment);

	/*
	 * Tags use with FInv_LabeledNumberFragment
	 * This determining display text with formating (Label + Value)
	 */
	
		
	/*
	 * Tags used with FInv_ScrollableLabelNumberFragment
	 * These determine the display of an array of label and values
	 * producing an FInv_LabelNumberFragment for each (Label + Value) to be used
	 * in a scrollbox.
	 */
	

	/*
	 * Tags used with FInv_AbilitySystemModifier a child of FInv_ScrollableLabelNumberFragment
	 * Used with Gameplay Ability System (GAS). Determine the display of the modifiers from the GameplayEffect
	 * in a scroll box using FInv_LabelNumberFragment for each row of the scroll box.
	 */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ModifierStatGroupOne);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ModifierStatGroupTwo);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ModifierStatGroupThree);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ModifierStatGroupFour);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ModifierStatGroupFive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ModifierStatGroupSix);

	/*
	* Tags used with FInv_AbilitySystemModifier a child of FInv_ScrollableLabelNumberFragment
	* Used with Gameplay Ability System (GAS). Determine the display of the modifiers from the GameplayAbilities
	* in a scroll box using FInv_LabelNumberFragment for each row of the scroll box.
	*/

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ModifierAbilityOne);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ModifierAbilityTwo);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ModifierAbilityThree);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ModifierAbilityFour);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ModifierAbilityFive);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(ModifierAbilitySix);
}