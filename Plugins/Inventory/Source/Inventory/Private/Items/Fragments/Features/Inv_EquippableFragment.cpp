#include "Items/Fragments/Features/Inv_EquippableFragment.h"
#include "Items/Fragments/AbilitySystem/Inv_AbilitySystemModifierFragment.h"


void FInv_EquippableFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
}

void FInv_EquippableFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	for ( const FInv_AbilitySystemModifierFragment& Modifer : AbilityModifiers)
	 {
	 	Modifer.Assimilate(Composite);
	 }
	FInv_InventoryItemFragment::Assimilate(Composite);
}

FInv_AbilitySystemModifierFragment* FInv_EquippableFragment::FindModifer(int32 Index)
{
	if (AbilityModifiers.IsValidIndex(Index))
	 {
	 	return &AbilityModifiers[Index];
	 }

	return nullptr;
}

FInv_AbilitySystemModifierFragment* FInv_EquippableFragment::FindModiferByTag(const FGameplayTag& Tag)
{
	if (AbilityModifiers.Num() == 0) return nullptr;
	
	for (auto& Data : AbilityModifiers )
	{
		FInv_AbilitySystemModifierFragment* Modifer = &Data;
		if ( Data.GetAbilityData().EffectTag == Tag )
		{
			return Modifer;
		}
	
		if ( Data.GetAbilityData().AbilityTag == Tag )
		{
			return Modifer;
		}
	}

	return nullptr;
}

void FInv_EquippableFragment::OnEquip(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;

	if (AbilityModifiers.Num() == 0) return;
	
	for ( auto& Data : AbilityModifiers )
	{
		Data.ApplyModifer(PlayerController, Item);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Equipped"));
}

void FInv_EquippableFragment::OnUnequip(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;

	if (AbilityModifiers.Num() == 0) return;
	
	for ( auto& Data : AbilityModifiers )
	{
		Data.RemoveModifer(PlayerController, Item);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("UnEquipped"));
}