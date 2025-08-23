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
	DelayedInitializeOwner();
}

void AInv_ProxyMeshActor::DelayedInitializeOwner()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		DelayedInitialization();
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!IsValid(PlayerController))
	{
		DelayedInitialization();
		return;
	}

	ACharacter* Character = Cast<ACharacter>(PlayerController->GetPawn());
	if (!IsValid(Character))
	{
		DelayedInitialization();
		return;
	}

	USkeletalMeshComponent* CharacterSkeletalMeshComponent = Character->GetMesh();
	if (!IsValid(CharacterSkeletalMeshComponent))
	{
		DelayedInitialization();
		return;
	}

	SourceSkeletalMeshComponent = CharacterSkeletalMeshComponent;
	ProxySkeletalMeshComponent->SetSkeletalMesh(SourceSkeletalMeshComponent->GetSkeletalMeshAsset());
	ProxySkeletalMeshComponent->SetAnimInstanceClass(SourceSkeletalMeshComponent->GetAnimInstance()->GetClass());

	EquipmentComponent->InitializeOwner(PlayerController);
}

void AInv_ProxyMeshActor::DelayedInitialization()
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this,&ThisClass::DelayedInitializeOwner);
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
}
