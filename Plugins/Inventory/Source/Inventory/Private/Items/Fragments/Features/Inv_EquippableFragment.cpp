#include "Items/Fragments/Features/Inv_EquippableFragment.h"

#include "EquipmentManagement/EquipActor/Inv_EquipActor.h"
#include "Items/Fragments/AbilitySystem/Inv_AbilityModifierFragment.h"
#include "Items/Fragments/AbilitySystem/Inv_AbilitySystemModifierFragment.h"
#include "Items/Fragments/AbilitySystem/Inv_EffectModifierFragment.h"
#include "Widgets/Composite/Inv_CompositeBase.h"


void FInv_EquippableFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();

	for ( TInstancedStruct<FInv_EffectModifierFragment>& Modifier : EffectModifiers)
	{
		if (Modifier.IsValid())
		{
			FInv_EffectModifierFragment&  ModifierInstance = Modifier.GetMutable();
			ModifierInstance.Manifest();
		}
	}
}

void FInv_EquippableFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	bool bContainerMatches = MatchesWidgetTag(Composite);
	
	for ( const TInstancedStruct<FInv_EffectModifierFragment>& Modifer : EffectModifiers)
	{
		if (Modifer.IsValid())
		{
			const FInv_EffectModifierFragment& Modifier = Modifer.Get();

			if ( Modifer.Get().HasFragmentTag(Composite->GetFragmentTag()))
			{
				Modifier.Assimilate(Composite);
			}
			
		}
	}
}


void FInv_EquippableFragment::OnEquip(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if ( !IsValid(PlayerController) ) return;

	if (EffectModifiers.Num() > 0 )
	{
		for (TInstancedStruct<FInv_EffectModifierFragment>& Modifer : EffectModifiers)
		{
			if (Modifer.IsValid())
			{
				Modifer.GetMutable<FInv_EffectModifierFragment>().OnApply(PlayerController, Item);
			}

		}
	}

	if ( Abilities.Num() > 0)
	{
		for (TInstancedStruct<FInv_AbilityModifierFragment>& Ability : Abilities)
		{
			Ability.GetMutable<FInv_AbilityModifierFragment>().OnApply(PlayerController, Item);
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Equipped"));
}

void FInv_EquippableFragment::OnUnequip(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (EffectModifiers.Num() > 0 )
	{
		for (TInstancedStruct<FInv_EffectModifierFragment>& Modifer : EffectModifiers)
		{
			if (Modifer.IsValid())
			{
				Modifer.GetMutable<FInv_EffectModifierFragment>().OnRemove(PlayerController, Item);
			}

		}
	}

	if ( Abilities.Num() > 0)
	{
		for (TInstancedStruct<FInv_AbilityModifierFragment>& Ability : Abilities)
		{
			Ability.GetMutable<FInv_AbilityModifierFragment>().OnRemove(PlayerController, Item);
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("UnEquipped"));
}

AInv_EquipActor* FInv_EquippableFragment::SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const
{
	if (!IsValid(EquipActorClass) || !IsValid(AttachMesh)) return nullptr;

	AInv_EquipActor* SpawnedActor = AttachMesh->GetWorld()->SpawnActor<AInv_EquipActor>(EquipActorClass);
	SpawnedActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketAttachPoint);
	return SpawnedActor;
}

void FInv_EquippableFragment::DestroyAttachedActor()
{
	if (EquippedActor.IsValid())
	{
		EquippedActor->Destroy();
		EquippedActor.Reset();
	}
}

void FInv_EquippableFragment::SetEquippedActor(AInv_EquipActor* EquipActor)
{
	DestroyAttachedActor();
	EquippedActor = TWeakObjectPtr<AInv_EquipActor>(EquipActor);
}
