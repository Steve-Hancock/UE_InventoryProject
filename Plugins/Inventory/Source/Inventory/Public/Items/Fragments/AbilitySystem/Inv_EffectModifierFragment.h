#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Items/Fragments/AbilitySystem/Inv_AbilitySystemModifierFragment.h"
#include "Inv_EffectModifierFragment.generated.h"

USTRUCT(BlueprintType)
struct FInv_EffectModifierFragment : public FInv_AbilitySystemModifierFragment
{
	GENERATED_BODY()

public:
	virtual void Manifest() override;
	virtual void Assimilate(UInv_CompositeBase* Composite) const override;
	virtual void OnApply(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);
	virtual void OnRemove(APlayerController* PlayerController, UInv_InventoryItem* Item = nullptr);

	UGameplayEffect* GetGameplayEffect() const {return GameplayEffectClass->GetDefaultObject<UGameplayEffect>();};
	FGameplayTag GetEffectTag() const {return EffectTag;}
	float GetEffectLevel() const {return EffectLevel;}
	FActiveGameplayEffectHandle GetActiveEffectHandle() const {return ActiveEffectHandle;}
	UGameplayCueNotify_Static* GetEffectCueClass() const {return EffectCueClass.GetDefaultObject();}
	FGameplayTag GetEffectCueTag() const {return EffectCueTag;};
	bool IsPercent() const {return UsePercent;}

	TSubclassOf<UInv_Leaf_LabeledValue> GetLabeledValueClass() const { return LabeledValueClass; }

	void SetEffectLevel(float NewLevel) {EffectLevel = NewLevel;}



private:

	//Gameplay Effect Related
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UGameplayCueNotify_Static> EffectCueClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EffectTag;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float EffectLevel{1.0f};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag EffectCueTag;

	UPROPERTY()
	bool UsePercent{false};
	
	UPROPERTY()
	TMap<FGameplayTag, float> SetByCallerMagnitudes;

	UPROPERTY()
	FActiveGameplayEffectHandle ActiveEffectHandle;;

	float GetEffectMagnitude(int32 Index) const;

	// Display Related

	// Effects can hold multiple effects based off multiple attributes, 
	// each attribute and the magnitude they have should be displayed on a different line.
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_Leaf_LabeledValue> LabeledValueClass;
	
	//Map to configure display options for each attribute
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ToolTip = "Configure display options for attributes. Key = Attribute Name"))
	TMap<FString, FAttributeDisplayOptions> AttributeDisplayOptions;

	FText FormatAttributeValue(float Value, const FAttributeDisplayOptions& Options) const;
	
};



