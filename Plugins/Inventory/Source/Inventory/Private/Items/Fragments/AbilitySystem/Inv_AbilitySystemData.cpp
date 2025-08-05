#include "Items/Fragments/AbilitySystem/Inv_AbilitySystemData.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayCueNotify_Static.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"


FGameplayAbilitySpecHandle FInv_AbilitySystemData::ApplyAbilityAndAbilityCue(APlayerController* PlayerController)
{
	FGameplayAbilitySpecHandle AbilityHandle;
	if (!IsValid(PlayerController)) return AbilityHandle;
	UAbilitySystemComponent* AbilitySystemComponent = UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController);
	if (!IsValid(AbilitySystemComponent)) return AbilityHandle;

	// Grant Gameplay Abilities
	if (!IsValid(AbilityToGrant)) return AbilityHandle;
	FGameplayAbilitySpec TempAbilitySpec(
		AbilityToGrant.Get(),
		AbilityLevel,
		INDEX_NONE,
		PlayerController->GetPawn());
	AbilityHandle = AbilitySystemComponent->GiveAbilityAndActivateOnce(TempAbilitySpec);
	
	return AbilityHandle;
}

FGameplayAbilitySpecHandle FInv_AbilitySystemData::ApplyAbilityAndAbilityCuePermanently(
	APlayerController* PlayerController)
{
	FGameplayAbilitySpecHandle AbilityHandle;
	if (!IsValid(PlayerController)) return AbilityHandle;
	UAbilitySystemComponent* AbilitySystemComponent = UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController);
	if (!IsValid(AbilitySystemComponent)) return AbilityHandle;

	// Remove existing ability if one is active
	if (ActiveAbilityHandle.IsValid())
	{
		RemoveAbility(PlayerController);
	}
	
	// Grant Gameplay Abilities
	if (!IsValid(AbilityToGrant)) return AbilityHandle;
	FGameplayAbilitySpec TempAbilitySpec(
		AbilityToGrant.Get(),
		AbilityLevel,
		INDEX_NONE,
		PlayerController->GetPawn());
	
	AbilityHandle = AbilitySystemComponent->GiveAbility(TempAbilitySpec);

	// Trigger Gameplay cue for ability if it exists
	if (IsValid(AbilityCueClass))
	{
		FGameplayCueParameters Parameters;
		Parameters.Instigator = PlayerController->GetPawn();
		Parameters.AbilityLevel = AbilityLevel;
		
		AbilitySystemComponent->ExecuteGameplayCue(AbilityCueTag, Parameters);
	}
	ActiveAbilityHandle = AbilityHandle;
	return AbilityHandle;
}

FActiveGameplayEffectHandle FInv_AbilitySystemData::ApplyEffectAndEffectCue(APlayerController* PlayerController)
{
	FActiveGameplayEffectHandle EffectHandle;
	if (!IsValid(PlayerController)) return EffectHandle;
	UAbilitySystemComponent* AbilitySystemComponent = UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController);
	if (!IsValid(AbilitySystemComponent)) return EffectHandle;
	
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

	// Remove exisitng effect if one is active
	if (ActiveEffectHandle.IsValid())
	{
		RemoveEffect(PlayerController);
	}
	
	if (IsValid(GameplayEffectClass) && EffectContext.IsValid())
	{
		
		// Create a gameplay effect spec
		
		const FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			GameplayEffectClass.Get(),
			EffectLevel,
			EffectContext);

		if (EffectSpecHandle.IsValid())
		{
			for (const TPair<FGameplayTag, float>& Pair : SetByCallerMagnitudes)
			{
				EffectSpecHandle.Data->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
			}
			
			FActiveGameplayEffectHandle NewEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
			EffectHandle = NewEffectHandle;
		}
	}

	// Trigger Gameplay cue for effect if it exists
	if (IsValid(EffectCueClass))
	{
		FGameplayCueParameters Parameters;
		Parameters.Instigator = PlayerController->GetPawn();
		Parameters.AbilityLevel = EffectLevel;
		Parameters.EffectCauser = PlayerController;
		Parameters.EffectContext = EffectContext;
		
		AbilitySystemComponent->ExecuteGameplayCue(EffectCueTag, Parameters);
	}
	
	ActiveEffectHandle = EffectHandle;
	return EffectHandle;
}

void FInv_AbilitySystemData::RemoveEffect(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return;
	UAbilitySystemComponent* AbilitySystemComponent = UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController);
	if (!IsValid(AbilitySystemComponent)) return;
	if (!ActiveEffectHandle.IsValid()) return;
	AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
	ActiveEffectHandle = FActiveGameplayEffectHandle();
}

void FInv_AbilitySystemData::RemoveAbility(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return;
	UAbilitySystemComponent* AbilitySystemComponent = UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController);
	if (!IsValid(AbilitySystemComponent)) return;
	if (!ActiveAbilityHandle.IsValid()) return;
	AbilitySystemComponent->ClearAbility(ActiveAbilityHandle);
	ActiveAbilityHandle = FGameplayAbilitySpecHandle();
}

float FInv_AbilitySystemData::GetEffectMagnitude(int32 Index) const
{
	if (!IsValid(GameplayEffectClass)) return 0.f;
	UGameplayEffect* GameplayEffect = GameplayEffectClass.GetDefaultObject();
	if (!IsValid(GameplayEffect)) return 0.f;
	if (!GameplayEffect->Modifiers.IsValidIndex(Index)) return 0.f;
	
	FGameplayEffectSpec TempSpec(GameplayEffect, FGameplayEffectContextHandle(), EffectLevel);
	const auto& Modifier = GameplayEffect->Modifiers[Index];

	// Inject SetByCaller values if needed BEFORE magnitude calculation
	if (Modifier.ModifierMagnitude.GetMagnitudeCalculationType() == EGameplayEffectMagnitudeCalculation::SetByCaller)
	{
		for (const TPair<FGameplayTag, float>& Pair : SetByCallerMagnitudes)
		{
			TempSpec.SetSetByCallerMagnitude(Pair.Key, Pair.Value);
		}
	}

	float Magnitude = 0.f;
	if (Modifier.ModifierMagnitude.AttemptCalculateMagnitude(TempSpec, Magnitude))
	{
		return Magnitude;
	}

	return 0.f;
}
