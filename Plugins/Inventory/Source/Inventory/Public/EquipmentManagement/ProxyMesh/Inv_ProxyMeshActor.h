// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inv_ProxyMeshActor.generated.h"

class UInv_EquipmentComponent;

UCLASS()
class INVENTORY_API AInv_ProxyMeshActor : public AActor
{
	GENERATED_BODY()

public:

	AInv_ProxyMeshActor();

protected:
	virtual void BeginPlay() override;

private:
	// This is the mesh on the player-controlled Character.
	TWeakObjectPtr<USkeletalMeshComponent> SourceSkeletalMeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UInv_EquipmentComponent> EquipmentComponent;

	// This is the proxy Mesh we will se in the Inventory Menu.
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<USkeletalMeshComponent> ProxySkeletalMeshComponent;

	FTimerHandle TimerForNextTick;
	void DelayedintializeOwner();
	void DelayedInitializeation();
};
