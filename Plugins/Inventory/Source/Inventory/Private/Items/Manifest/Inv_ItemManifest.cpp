#include "Items/Manifest/Inv_ItemManifest.h"

#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Widgets/Composite/Inv_CompositeBase.h"

UInv_InventoryItem* FInv_ItemManifest::Manifest(UObject* NewOuter)
{
	UInv_InventoryItem* NewItem = NewObject<UInv_InventoryItem>(NewOuter, UInv_InventoryItem::StaticClass());
	NewItem->SetItemManifest(*this);

	for (auto& Fragment : NewItem->GetItemManifestMutable().GetFragmentsMutable())
	{
		Fragment.GetMutable().Manifest();
	}
	ClearFragments();
	return NewItem;
}

void FInv_ItemManifest::AssimilateInventoryFragments(UInv_CompositeBase* Composite) const
{
	const auto& Frags = GetAllFragmentsOfType<FInv_InventoryItemFragment>();
	for (const auto* Fragment : Frags)
	{
		Composite->ApplyFunction([Fragment](UInv_CompositeBase* Widget)
		{
			Fragment->Assimilate(Widget);
		});
	}
}

void FInv_ItemManifest::SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation,
                                         const FRotator& SpawnRotation)
{
	if (!IsValid(WorldContextObject) || !IsValid(PickupActorClass)) return;

	AActor* SpawnedActor = WorldContextObject->GetWorld()->SpawnActor<AActor>(PickupActorClass, SpawnLocation, SpawnRotation);
	if (!IsValid(SpawnedActor)) return;

	// Set the item manifest, item category, item type, ect.
	UInv_ItemComponent* ItemComponent = SpawnedActor->FindComponentByClass<UInv_ItemComponent>();
	check(ItemComponent);

	ItemComponent->InitItemManifest(*this);
}

void FInv_ItemManifest::ClearFragments()
{
	for (auto& Fragment : Fragments)
	{
		Fragment.Reset();
	}
	Fragments.Empty();
}
