#include "AbilitySystemComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Fragments/AbilitySystem/Inv_AbilityModifierFragment.h"

void FInv_AbilityModifierFragment::Manifest()
{
	FInv_AbilitySystemModifierFragment::Manifest();
}

void FInv_AbilityModifierFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_AbilitySystemModifierFragment::Assimilate(Composite);
}

void FInv_AbilityModifierFragment::OnApply(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;
	UGameplayAbility* AbilityToGrant = GetGameplayAbilityClass();
	if (!IsValid(AbilityToGrant)) return;
	FGameplayAbilitySpecHandle AbilitySpecHandle;
	UAbilitySystemComponent* AbilitySystemComponent = UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController);
	if (!IsValid(AbilitySystemComponent)) return;

	
	if ( IsPermanent())
	{
		// Remove existing ability if one is active
		if (ActiveAbilitySpecHandle.IsValid())
		{
			OnRemove(PlayerController);
		}
	
		// Grant Gameplay Abilities
		FGameplayAbilitySpec TempAbilitySpec(
			GameplayAbilityClass,
			AbilityLevel,
			INDEX_NONE,
			PlayerController->GetPawn());
	
		AbilitySpecHandle = AbilitySystemComponent->GiveAbility(TempAbilitySpec);

		if ( IsActivateAbilityImmediately())
		{
			ActivateAbilityImmediately(PlayerController, AbilitySpecHandle);
		}

		// Trigger Gameplay cue for ability if it exists
		TriggerAbilityCue(PlayerController, AbilitySpecHandle);
		ActiveAbilitySpecHandle = AbilitySpecHandle;
	}
	else
	{
		// Grant Gameplay Abilities
		FGameplayAbilitySpec TempAbilitySpec(
			GameplayAbilityClass,
			AbilityLevel,
			INDEX_NONE,
			PlayerController->GetPawn());
		AbilitySpecHandle = AbilitySystemComponent->GiveAbilityAndActivateOnce(TempAbilitySpec);
		ActiveAbilitySpecHandle = AbilitySpecHandle;
		TriggerAbilityCue(PlayerController, AbilitySpecHandle);
	}
}

void FInv_AbilityModifierFragment::OnRemove(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;
	UAbilitySystemComponent* AbilitySystemComponent = UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController);
	if (!IsValid(AbilitySystemComponent)) return;
	if (!ActiveAbilitySpecHandle.IsValid()) return;
	AbilitySystemComponent->ClearAbility(ActiveAbilitySpecHandle);
	ActiveAbilitySpecHandle = FGameplayAbilitySpecHandle();
}

void FInv_AbilityModifierFragment::ActivateAbilityImmediately(APlayerController* PlayerController, FGameplayAbilitySpecHandle ActiveAbility)
{
	UAbilitySystemComponent* AbilitySystemComponent = UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController);
	if (!IsValid(AbilitySystemComponent)) return;
	if (ActiveAbility.IsValid())
	{
		AbilitySystemComponent->TryActivateAbility(ActiveAbility);
	}
}

void FInv_AbilityModifierFragment::TriggerAbilityCue(APlayerController* PlayerController, FGameplayAbilitySpecHandle ActiveAbility)
{
	if (!IsValid(PlayerController)) return;
	UAbilitySystemComponent* AbilitySystemComponent = UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController);
	if (!IsValid(AbilitySystemComponent)) return;
	
	if (ActiveAbility.IsValid())
	{
		FGameplayCueParameters Parameters;
		Parameters.Instigator = PlayerController->GetPawn();
		Parameters.AbilityLevel = AbilityLevel;
		
		AbilitySystemComponent->ExecuteGameplayCue(AbilityCueTag, Parameters);
	}
}
