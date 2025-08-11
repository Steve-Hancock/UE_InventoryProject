// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"

#include "EquipmentManagement/EquipActor/Inv_EquipActor.h"
#include "GameFramework/Character.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Fragments/Features/Inv_EquippableFragment.h"
#include "Items/Manifest/Inv_ItemManifest.h"


void UInv_EquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningPlayerController = Cast<APlayerController>(GetOwner());
	if (OwningPlayerController.IsValid())
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(OwningPlayerController->GetPawn());
		if (IsValid(OwnerCharacter))
		{
			OwningSkeletalMeshComponent = OwnerCharacter->GetMesh();
		}
		InitInventoryComponent();
	}
}

void UInv_EquipmentComponent::OnItemEquipped(UInv_InventoryItem* EquippedItem)
{
	if (!EquippedItem) return;
	if (!OwningPlayerController->HasAuthority()) return;

	FInv_ItemManifest& ItemManifest = EquippedItem->GetItemManifestMutable();
	FInv_EquippableFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FInv_EquippableFragment>();
	if (!EquipmentFragment) return;

	EquipmentFragment->OnEquip(OwningPlayerController.Get(), EquippedItem);

	if (!OwningSkeletalMeshComponent.IsValid()) return;
	
	AInv_EquipActor* SpawnedEquipActor = SpawnEquippedActor(EquipmentFragment, ItemManifest,
		OwningSkeletalMeshComponent.Get());

	EquippedActors.Add(SpawnedEquipActor);
	
}

void UInv_EquipmentComponent::OnItemUnequipped(UInv_InventoryItem* UnEquippedItem)
{
	// Add stack trace or unique identifier
	UE_LOG(LogTemp, Warning, TEXT("OnUnequip called from: %s"), *FString(__FUNCTION__));

	if (!UnEquippedItem) return;
	if (!OwningPlayerController->HasAuthority()) return;

	FInv_ItemManifest& ItemManifest = UnEquippedItem->GetItemManifestMutable();
	FInv_EquippableFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FInv_EquippableFragment>();
	if (!EquipmentFragment) return;

	EquipmentFragment->OnUnequip(OwningPlayerController.Get(), UnEquippedItem);

	RemoveEquippedActorByTag(EquipmentFragment->GetEquipmentType());
}

void UInv_EquipmentComponent::InitInventoryComponent()
{
	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(OwningPlayerController.Get());
	if (!InventoryComponent.IsValid()) return;

	if (!InventoryComponent->OnItemEquipped.IsAlreadyBound(this, &ThisClass::OnItemEquipped))
	{
		InventoryComponent->OnItemEquipped.AddDynamic(this, &ThisClass::OnItemEquipped);
	}

	if (!InventoryComponent->OnItemUnequipped.IsAlreadyBound(this, &ThisClass::OnItemUnequipped))
	{
		InventoryComponent->OnItemUnequipped.AddDynamic(this, &ThisClass::OnItemUnequipped);
	}
}

AInv_EquipActor* UInv_EquipmentComponent::SpawnEquippedActor(FInv_EquippableFragment* EqquippableFragment,
	const FInv_ItemManifest& Manifest, USkeletalMeshComponent* AttachMesh)
{
	AInv_EquipActor* SpawnedEquippedActor = EqquippableFragment->SpawnAttachedActor(AttachMesh);
	SpawnedEquippedActor->SetEquipmentTypeTag(EqquippableFragment->GetEquipmentType());
	SpawnedEquippedActor->SetOwner(GetOwner());
	EqquippableFragment->SetEquippedActor(SpawnedEquippedActor);
	return SpawnedEquippedActor;
}

AInv_EquipActor* UInv_EquipmentComponent::FindEquippedActorByTag(const FGameplayTag& EquipmentTypeTag)
{
	auto FoundActor = EquippedActors.FindByPredicate([&EquipmentTypeTag](const AInv_EquipActor* Actor)
	{
		return Actor->GetEquipmentTypeTag().MatchesTagExact(EquipmentTypeTag);
	});
	return FoundActor ? *FoundActor : nullptr;
}

void UInv_EquipmentComponent::RemoveEquippedActorByTag(const FGameplayTag& EquipmentTypeTag)
{
	if (AInv_EquipActor* FoundActor = FindEquippedActorByTag(EquipmentTypeTag); IsValid(FoundActor))
	{
		EquippedActors.Remove(FoundActor);
		FoundActor->Destroy();
	}
}

