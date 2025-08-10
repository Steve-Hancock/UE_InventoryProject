#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Items/Fragments/AbilitySystem/Inv_AbilitySystemModifierFragment.h"
#include "Inv_AbilityModifierFragment.generated.h"


USTRUCT(BlueprintType)
struct FInv_AbilityModifierFragment : public FInv_AbilitySystemModifierFragment
{
	GENERATED_BODY()

public:
	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	virtual void OnApply(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);
	virtual void OnRemove(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);

	UGameplayAbility* GetGameplayAbilityClass() const {return GameplayAbilityClass->GetDefaultObject<UGameplayAbility>();};
	FGameplayTag GetAbilityTag() const {return AbilityTag;}
	float GetAbilityLevel() const {return AbilityLevel;}
	FGameplayAbilitySpecHandle GetActiveAbilityHandle() const {return ActiveAbilitySpecHandle;}
	UGameplayCueNotify_Static* GetAbilityCueClass() const {return AbilityCueClass.GetDefaultObject();}
	FGameplayTag GetAbilityCueTag() const {return AbilityCueTag;};
	
	void SetAbilityLevelLevel(const float NewLevel) {AbilityLevel = NewLevel;}
	bool IsPermanent() const {return Permanent;}
	bool IsActivateAbilityImmediately() const {return ActivateImmediately;}

	

private:
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayAbility> GameplayAbilityClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayCueNotify_Static> AbilityCueClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag AbilityTag;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float AbilityLevel{1.0f};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag AbilityCueTag;

	UPROPERTY()
	bool Permanent{false};

	UPROPERTY()
	bool ActivateImmediately{false};
	
	UPROPERTY()
	FGameplayAbilitySpecHandle ActiveAbilitySpecHandle;

	void ActivateAbilityImmediately(APlayerController* PlayerController, FGameplayAbilitySpecHandle ActiveAbility);
	void TriggerAbilityCue(APlayerController* PlayerController, FGameplayAbilitySpecHandle ActiveAbility);
	
};


