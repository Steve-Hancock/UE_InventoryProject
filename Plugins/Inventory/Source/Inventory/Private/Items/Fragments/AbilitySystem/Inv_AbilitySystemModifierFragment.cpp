#include "Items/Fragments/AbilitySystem/Inv_AbilitySystemModifierFragment.h"

#include "AbilitySystemComponent.h"
#include "Components/VerticalBox.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Fragments/Core/Inv_MultipleLabeledValueFragment.h"
#include "Widgets/Composite/Inv_Leaf_MultipleLabeledValues.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"

void FInv_AbilitySystemModifierFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
}

void FInv_AbilitySystemModifierFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);

	if (!MatchesWidgetTag(Composite)) return;
	
	UInv_Leaf_MultipleLabeledValues* MultipleLabeledValues = Cast<UInv_Leaf_MultipleLabeledValues>(Composite);
	if (!IsValid(MultipleLabeledValues)) return;

	MultipleLabeledValues->GetVerticalBox()->ClearChildren();

	if (AbilityData.GameplayEffectClass == nullptr) return;
	
	UGameplayEffect* GameplayEffect = AbilityData.GameplayEffectClass->GetDefaultObject<UGameplayEffect>();
	if (!IsValid(GameplayEffect)) return;

	for (int32 Index = 0; Index < GameplayEffect->Modifiers.Num(); ++Index)
	{
		UInv_Leaf_LabeledValue* LabeledValue = CreateWidget<UInv_Leaf_LabeledValue>(Composite, GetLabeledValueClass());
		if (!IsValid(LabeledValue)) continue;

		const FGameplayModifierInfo& ModifierInfo = GameplayEffect->Modifiers[Index];
		FString AttributeName = ModifierInfo.Attribute.AttributeName;
		float Value = AbilityData.GetEffectMagnitude(Index);

		// Get display options for this attribute
		FAttributeDisplayOptions DisplayOptions;
		if (AttributeDisplayOptions.Contains(AttributeName))
		{
			DisplayOptions = AttributeDisplayOptions[AttributeName];
		}

		// Determine display name
		FText DisplayName;
		if (!DisplayOptions.DisplayName.IsEmpty())
		{
			DisplayName = DisplayOptions.DisplayName;
		}
		else
		{
			DisplayName = FText::FromString(AttributeName);
		}

		// Add colon to label
		FString DisplayString = DisplayName.ToString();
		if (!DisplayString.EndsWith(TEXT(":")))
		{
			DisplayString += TEXT(":");
			DisplayName = FText::FromString(DisplayString);
		}

		// Format the value
		FText FormattedValue = FormatAttributeValue(Value, DisplayOptions);

		LabeledValue->SetTextLabel(DisplayName, false);
		LabeledValue->SetTextValue(FormattedValue, false);
		
		MultipleLabeledValues->GetVerticalBox()->AddChild(LabeledValue);
		
	}
}



void FInv_AbilitySystemModifierFragment::ApplyModifer(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;

	// Apply GameplayEffect if specified
	if (AbilityData.GameplayEffectClass)
	{
		AbilityData.ApplyEffectAndEffectCue(PlayerController);
	}

	// Grant/Apply Ability if specified
	if (AbilityData.AbilityToGrant) // This is the bool check
	{
		if (AbilityData.GrantAbilityPermanently)
		{
			AbilityData.ApplyAbilityAndAbilityCuePermanently(PlayerController);
			if (AbilityData.ActivateAbilityImmediately)
			{
				UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController)->TryActivateAbility(AbilityData.ActiveAbilityHandle);
			}
		}
		else
		{
			AbilityData.ApplyAbilityAndAbilityCue(PlayerController);
		}
	}
}

void FInv_AbilitySystemModifierFragment::RemoveModifer(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;
	if (AbilityData.ActiveAbilityHandle.IsValid() && AbilityData.AbilityToGrant)
	{
		AbilityData.RemoveAbility(PlayerController);
	}

	if (AbilityData.ActiveEffectHandle.IsValid() && AbilityData.GameplayEffectClass)
	{
		AbilityData.RemoveEffect(PlayerController);
	}
}

FText FInv_AbilitySystemModifierFragment::FormatAttributeValue(float Value, const FAttributeDisplayOptions& Options) const
{
	FString ValueString;

	// Handle percentage conversion
	float DisplayValue = Options.bShowPercentage ? Value * 100.0f : Value;

	// Add +/- prefix
	if (Options.bShowSign)
	{
		if (DisplayValue > 0.0f)
		{
			ValueString = TEXT("+");
		}
		// Negative values automatically have the minus sign
	}

	// Add the number
	ValueString += FString::Printf(TEXT("%.1f"), DisplayValue);

	// Add percentage symbol
	if (Options.bShowPercentage)
	{
		ValueString += TEXT("%");
	}

	return FText::FromString(ValueString);

}
