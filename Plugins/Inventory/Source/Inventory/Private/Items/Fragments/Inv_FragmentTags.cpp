#include "Items/Fragments/Inv_FragmentTags.h"

namespace FragmentTags
{
	/*
	* Base Function Fragment Tags
	* These tags map directly a specific fragment struct
	*/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GridFragment, "FragmentTags.GridFragment", "Size of the Inventory grid items takes up ex 1 x 1");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ImageFragment, "FragmentTags.ImageFragment", "The Icon that represents the Item");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(StackableFragment, "FragmentTags.StackableFragment", "Items that can be stacked");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ConsumableFragment, "FragmentTags.ConsumableFragment", "Items that can be consumed");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(EquippableFragment, "FragmentTags.EquippableFragment", "Items that can be equipped");

	/*
	* Tags use with FInv_TextFragment
	* These determine display text, but not structure
	*/ 
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ItemNameFragment, "FragmentTags.ItemNameFragment", "The name of the Item");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FlavorTextFragment, "FragmentTags.FlavorTextFragment", "The flavor text of the Item");

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
	 * Tag used with FInv_AbilitySystemModifier a child of FInv_ScrollableLabelNumberFragment
	 * Used with Gameplay Ability System (GAS). Determine the display of the modifiers from the GameplayEffect
	 * in a scroll box using FInv_LabelNumberFragment for each row of the scroll box.
	 */
	
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ModifierStatGroupOne, "FragmentTags.ModifierStatGroupOne", "Primary Stat Group One");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ModifierStatGroupTwo, "FragmentTags.ModifierStatGroupTwo", "Primary Stat Group Two");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ModifierStatGroupThree, "FragmentTags.ModifierStatGroupThree", "Primary Stat Group Three");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ModifierStatGroupFour, "FragmentTags.ModifierStatGroupFour", "Primary Stat Group Four");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ModifierStatGroupFive, "FragmentTags.ModifierStatGroupFive", "Primary Stat Group Five");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ModifierStatGroupSix, "FragmentTags.ModifierStatGroupSix", "Primary Stat Group Six");

	/*
	* Tags used with FInv_AbilitySystemModifier a child of FInv_ScrollableLabelNumberFragment
	* Used with Gameplay Ability System (GAS). Determine the display of the modifiers from the GameplayAbilities
	* in a scroll box using FInv_LabelNumberFragment for each row of the scroll box.
	*/
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ModifierAbilityOne, "FragmentTags.ModifierAbilityOne", "Ability One");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ModifierAbilityTwo, "FragmentTags.ModifierAbilityTwo", "Ability Two");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ModifierAbilityThree, "FragmentTags.ModifierAbilityThree", "Ability Three");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ModifierAbilityFour, "FragmentTags.ModifierAbilityFour", "Ability Four");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ModifierAbilityFive, "FragmentTags.ModifierAbilityFive", "Ability Five");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ModifierAbilitySix, "FragmentTags.ModifierAbilitySix", "Ability Six");
	
}