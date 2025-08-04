// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Inv_ItemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORY_API UInv_ItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInv_ItemComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitItemManifest(FInv_ItemManifest CopyOfManifest);
	FInv_ItemManifest GetItemManifest() const { return ItemManifest; };
	FString GetPickupMessage() const { return PickupMessage; };
	void PickedUp();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnPickedUp();

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FString PickupMessage;

	UPROPERTY(Replicated, EditAnywhere, Category = "Inventory")
	FInv_ItemManifest ItemManifest;
};
