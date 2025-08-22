// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.


#include "EquipmentManagement/ProxyMesh/Inv_ProxyMeshActor.h"
#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"
#include "GameFramework/Character.h"


// Sets default values
AInv_ProxyMeshActor::AInv_ProxyMeshActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// create the root
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	ProxySkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>("ProxySkeletalMeshComponent");
	ProxySkeletalMeshComponent->SetupAttachment(RootComponent);

	EquipmentComponent = CreateDefaultSubobject<UInv_EquipmentComponent>("EquipmentComponent");
	EquipmentComponent->SetOwningSkeletalMeshComponent(ProxySkeletalMeshComponent);
	EquipmentComponent->SetIsProxy(true);
}

void AInv_ProxyMeshActor::BeginPlay()
{
	Super::BeginPlay();
	DelayedintializeOwner();
}

void AInv_ProxyMeshActor::DelayedintializeOwner()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		DelayedInitializeation();
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		DelayedInitializeation();
		return;
	}

	ACharacter* Character = Cast<ACharacter>(PlayerController->GetPawn());
	if (!IsValid(PlayerController))
	{
		DelayedInitializeation();
		return;
	}

	USkeletalMeshComponent* CharacterSkeletalMeshComponent = Character->GetMesh();
	if (!IsValid(CharacterSkeletalMeshComponent))
	{
		DelayedInitializeation();
		return;
	}

	SourceSkeletalMeshComponent = CharacterSkeletalMeshComponent;
	ProxySkeletalMeshComponent->SetSkeletalMesh(SourceSkeletalMeshComponent->GetSkeletalMeshAsset());
	ProxySkeletalMeshComponent->SetAnimInstanceClass(SourceSkeletalMeshComponent->GetAnimInstance()->GetClass());

	EquipmentComponent->InitializeOwner(PlayerController);
}

void AInv_ProxyMeshActor::DelayedInitializeation()
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this,&ThisClass::DelayedintializeOwner);
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
}
