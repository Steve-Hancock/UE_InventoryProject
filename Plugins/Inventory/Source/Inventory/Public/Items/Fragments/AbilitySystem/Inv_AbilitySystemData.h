#pragma once
#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "Inv_AbilitySystemData.generated.h"


class UGameplayAbility;
class UGameplayCueNotify_Static;


USTRUCT(BlueprintType)
struct FInv_AbilitySystemData
{
	GENERATED_BODY()

	// Effects 
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EffectTag;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float EffectLevel = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EffectCueTag;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayCueNotify_Static> EffectCueClass;

	// Abilities
	UPROPERTY(editAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayAbility> AbilityToGrant;

	UPROPERTY(editAnywhere, Category = "Inventory")
	FGameplayTag AbilityTag;

	UPROPERTY(editAnywhere, Category = "Inventory")
	float AbilityLevel = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool GrantAbilityPermanently = false;

	// Applies only to permanent abilities. Non-permanent abilities are always activated immediately.
	// If this is true, the permanent ability will also be activated once when it is granted (e.g., for passive or auto-triggered effects).
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool ActivateAbilityImmediately = false;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag AbilityCueTag;
	
	UPROPERTY(editAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayCueNotify_Static> AbilityCueClass;

	UPROPERTY()
	FActiveGameplayEffectHandle ActiveEffectHandle;;

	UPROPERTY()
	FGameplayAbilitySpecHandle ActiveAbilityHandle;
	
	UPROPERTY()
	TMap<FGameplayTag, float> SetByCallerMagnitudes;
	
	FGameplayAbilitySpecHandle ApplyAbilityAndAbilityCue(APlayerController* PlayerController);
	FGameplayAbilitySpecHandle ApplyAbilityAndAbilityCuePermanently(APlayerController* PlayerController);
	FActiveGameplayEffectHandle ApplyEffectAndEffectCue(APlayerController* PlayerController);
	void RemoveEffect(APlayerController* PlayerController);
	void RemoveAbility(APlayerController* PlayerController);
	auto GetEffectMagnitude(int32 Index) const -> float;
};
