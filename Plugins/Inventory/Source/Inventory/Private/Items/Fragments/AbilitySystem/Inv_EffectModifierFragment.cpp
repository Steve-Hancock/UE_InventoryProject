#include "Items/Fragments/AbilitySystem/Inv_EffectModifierFragment.h"

#include "AbilitySystemComponent.h"
#include "GameplayCueNotify_Static.h"
#include "Components/VerticalBox.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"
#include "Widgets/Composite/Inv_Leaf_MultipleLabeledValues.h"


void FInv_EffectModifierFragment::Manifest()
{
	FInv_AbilitySystemModifierFragment::Manifest();
}

void FInv_EffectModifierFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_AbilitySystemModifierFragment::Assimilate(Composite);

	if (!MatchesWidgetTag(Composite)) return;
	
	UInv_Leaf_MultipleLabeledValues* MultipleLabeledValues = Cast<UInv_Leaf_MultipleLabeledValues>(Composite);
	if (!IsValid(MultipleLabeledValues)) return;

	MultipleLabeledValues->GetVerticalBox()->ClearChildren();

	if (GameplayEffectClass == nullptr) return;
	
	UGameplayEffect* GameplayEffect = GameplayEffectClass->GetDefaultObject<UGameplayEffect>();
	if (!IsValid(GameplayEffect)) return;

	for (int32 Index = 0; Index < GameplayEffect->Modifiers.Num(); ++Index)
	{
		UInv_Leaf_LabeledValue* LabeledValue = CreateWidget<UInv_Leaf_LabeledValue>(Composite, GetLabeledValueClass());
		if (!IsValid(LabeledValue)) continue;

		const FGameplayModifierInfo& ModifierInfo = GameplayEffect->Modifiers[Index];
		FString AttributeName = ModifierInfo.Attribute.AttributeName;
		float Value = GetEffectMagnitude(Index);

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

		// Add colon to the label
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

void FInv_EffectModifierFragment::OnApply(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	FActiveGameplayEffectHandle EffectHandle;
	if (!IsValid(PlayerController)) return;
	UAbilitySystemComponent* AbilitySystemComponent = UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController);
	if (!IsValid(AbilitySystemComponent)) return;
	
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();

	// Remove existing effect if one is active
	if (ActiveEffectHandle.IsValid())
	{
		OnRemove(PlayerController);
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
}

void FInv_EffectModifierFragment::OnRemove(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;
	UAbilitySystemComponent* AbilitySystemComponent = UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController);
	if (!IsValid(AbilitySystemComponent)) return;
	if (!ActiveEffectHandle.IsValid()) return;
	AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
	ActiveEffectHandle = FActiveGameplayEffectHandle();
}


float FInv_EffectModifierFragment::GetEffectMagnitude(int32 Index) const
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

FText FInv_EffectModifierFragment::FormatAttributeValue(float Value, const FAttributeDisplayOptions& Options) const
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
