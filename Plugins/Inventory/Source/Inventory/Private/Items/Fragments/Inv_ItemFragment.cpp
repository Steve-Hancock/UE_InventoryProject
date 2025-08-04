#include "Items/Fragments/Inv_ItemFragment.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayCueNotify_Static.h"
#include "GameplayEffectTypes.h"
#include "Components/ScrollBox.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Widgets/Composite/Inv_CompositeBase.h"
#include "Widgets/Composite/Inv_Leaf_Image.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"
#include "Widgets/Composite/Inv_Leaf_Text.h"
#include "Widgets/Composite/Inv_Leaf_ScrollableLabeledValues.h"


#pragma region Inventory Item Fragment
void FInv_InventoryItemFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	Composite->Expand();
}

bool FInv_InventoryItemFragment::MatchesWidgetTag(const UInv_CompositeBase* Composite) const
{
	return HasFragmentTag(Composite->GetFragmentTag());
}

#pragma endregion

#pragma region Image Fragment
void FInv_ImageFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	UInv_Leaf_Image* Image = Cast<UInv_Leaf_Image>(Composite);
	if (!IsValid(Image)) return;

	Image->SetImage(Icon);
	Image->SetBoxSize(IconDimensions);
	Image->SetImageSize(IconDimensions);
}
#pragma endregion

#pragma region Consumable Fragment


void FInv_ConsumableFragment::Manifest()
{
	if (AbilityModifiers.Num() > 0)
	{
		bHasAbilityModifiers = true;
		for (FInv_AbilitySystemModifier& Modifer : AbilityModifiers)
		{
			Modifer.Manifest();
		}
	}

	FInv_InventoryItemFragment::Manifest();
}

void FInv_ConsumableFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	
	for ( const FInv_AbilitySystemModifier& Modifer : AbilityModifiers)
	{
		Modifer.Assimilate(Composite);
	}

	FInv_InventoryItemFragment::Assimilate(Composite);
}

FInv_AbilitySystemModifier* FInv_ConsumableFragment::FindModifer(int32 Index) 
{
	if (AbilityModifiers.IsValidIndex(Index))
	{
		return &AbilityModifiers[Index];
	}

	return nullptr;
}

FInv_AbilitySystemModifier* FInv_ConsumableFragment::FindModiferByTag(const FGameplayTag& Tag)
{
	if (AbilityModifiers.Num() == 0) return nullptr;
	
	for (auto& Data : AbilityModifiers )
	{
		FInv_AbilitySystemModifier* Modifer = &Data;
		if ( Data.AbilitySystemData.EffectTag == Tag )
		{
			return Modifer;
		}

		if ( Data.AbilitySystemData.AbilityTag == Tag )
		{
			return Modifer;
		}
	}

	return nullptr;
}

void  FInv_ConsumableFragment::OnConsume(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;

	if (AbilityModifiers.Num() == 0) return;
	
	for ( auto& Data : AbilityModifiers )
	{
		Data.ApplyModifier(PlayerController, Item);
	}
}


#pragma endregion

#pragma region Text Fragment
void FInv_TextFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;
	
	UInv_Leaf_Text* LeafText = Cast<UInv_Leaf_Text>(Composite);
	if (!IsValid(LeafText)) return;

	LeafText->SetText(GetText());
	
}
#pragma endregion

#pragma region Labeled Number Fragment

void FInv_LabeledNumberFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
	
	if (bRandomizeOnManifest)
	{
		Value = FMath::FRandRange(Min, Max);
	}
	
	bRandomizeOnManifest = false;
}

void FInv_LabeledNumberFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);

	if (!MatchesWidgetTag(Composite)) return;
	UInv_Leaf_LabeledValue* LabeledValue = Cast<UInv_Leaf_LabeledValue>(Composite);
	if (!IsValid(LabeledValue)) return;

	FText FinalLabelText = GetLabelText();
	if (!bCollapseLabel && !LabelText.IsEmpty())
	{
		FString LabelString = LabelText.ToString();
		{
			if (!LabelString.EndsWith(":"))
			{
				LabelString += ":";
			}
			FinalLabelText = FText::FromString(LabelString);
		}
	}
	LabeledValue->SetTextLabel(FinalLabelText, bCollapseLabel);

	FNumberFormattingOptions NumberFormattingOptions;
	NumberFormattingOptions.MinimumFractionalDigits = MinFractionalDigits;
	NumberFormattingOptions.MaximumFractionalDigits = MaxFractionalDigits;

	FText FormattedText;
	FString Sign = Value < 0 ? "-" : "+";
	FString ValueString = FText::AsNumber(FMath::Abs(Value), &NumberFormattingOptions).ToString();

	if (bIsPercent)
	{
		FormattedText = FText::FromString(Sign + ValueString + TEXT("%"));
	}
	else
	{
		FormattedText = FText::FromString(Sign + ValueString);
	}
	LabeledValue->SetTextValue(FormattedText, bCollapseValue);
}

void FInv_ScrollableLabelNumberFragment::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
}

void FInv_ScrollableLabelNumberFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
}


#pragma endregion

#pragma region Scrollable Labeled Values Fragment

#pragma endregion

#pragma region AbilitySystem Modifier

void FInv_AbilitySystemModifier::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	FInv_ScrollableLabelNumberFragment::Assimilate(Composite);

	UInv_Leaf_ScrollableLabeledValues* ScrollableLabeledValue = Cast<UInv_Leaf_ScrollableLabeledValues>(Composite);
	if (!IsValid(ScrollableLabeledValue)) return;

	ScrollableLabeledValue->GetScrollBox()->ClearChildren();

	if (AbilitySystemData.GameplayEffectClass == nullptr) return;
	UGameplayEffect* GameplayEffect = AbilitySystemData.GameplayEffectClass->GetDefaultObject<UGameplayEffect>();
	if (IsValid(GameplayEffect))
	{
		// Warn if Labels count doesn't match GameplayEffect modifiers when labels are provided
		if (GetLabels().Num() > 0 && GetLabels().Num() != GameplayEffect->Modifiers.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("UInv_Leaf_ScrollableLabeledValues: Labels count (%d) does not match GameplayEffect modifier count (%d). Ensure labels are in the same order as modifiers."), 
			GetLabels().Num(), GameplayEffect->Modifiers.Num());
		}

		FGameplayEffectContextHandle Context;
		const FGameplayEffectSpec TempSpec(GameplayEffect, Context, AbilitySystemData.EffectLevel);
		
		for (int32 Index = 0; Index < GameplayEffect->Modifiers.Num(); ++Index)
		{
			UInv_Leaf_LabeledValue* LabeledValue = CreateWidget<UInv_Leaf_LabeledValue>(Composite, GetLabeledValueClass());

			if (IsValid(LabeledValue))
			{
				FText LabelText;
				
				// Determine label text: use a custom label if provided and not "None", otherwise use attribute name
				if (GetLabels().IsValidIndex(Index) && !GetLabelText(Index).EqualToCaseIgnored(FText::FromString(TEXT("None"))))
				{
					LabelText = GetLabelText(Index);
					if (!LabelText.IsEmpty())
					{
						FString LabelString = LabelText.ToString();
						if (!LabelString.EndsWith(TEXT(":")))
						{
							LabelString += TEXT(":");
						}
						LabelText = FText::FromString(LabelString);
					}
				}
				else
				{
					// Use the attribute name from the GameplayEffect Modifier
					LabelText = FText::FromString(GameplayEffect->Modifiers[Index].Attribute.AttributeName);
					if (!LabelText.IsEmpty())
					{
						FString LabelString = LabelText.ToString();
						if (!LabelString.EndsWith(TEXT(":")))
						{
							LabelString += TEXT(":");
						}
						LabelText = FText::FromString(LabelString);
					}
				}
				
				LabeledValue->SetTextLabel(LabelText, false);

				// LabelValue

			

				FNumberFormattingOptions NumberFormattingOptions;
				NumberFormattingOptions.MinimumFractionalDigits = GetMinFractionalDigits();
				NumberFormattingOptions.MaximumFractionalDigits = GetMaxFractionalDigits();

				FText FormattedText;
				float Value = AbilitySystemData.GetEffectMagnitude(Index);
			
				// Convert multiplicative value (e.g., 1.25) to percentage increase (e.g., +25%)
				EGameplayModOp::Type ModifierType = GameplayEffect->Modifiers[Index].ModifierOp;
				if ( ModifierType == EGameplayModOp::Multiplicitive)
				{
					Value = (Value - 1) * 100;
				}

				FString Sign = Value < 0 ? TEXT("-") : TEXT("+");
				FString ValueString = FText::AsNumber(FMath::Abs(Value), &NumberFormattingOptions).ToString();

				// Safely add the % sign if marked as percent and the index is valid

				if (UsePercent.IsValidIndex(Index) && UsePercent[Index])
				{
					FormattedText = FText::FromString(Sign + ValueString + TEXT("%"));
				}
				else
				{
					FormattedText = FText::FromString(Sign + ValueString + TEXT("   "));
				}
				
				if (LabeledValue)
				{
					LabeledValue->SetTextValue(FormattedText, false);
					LabeledValue->SetFontSizeLabel(GetFontSizeLabels());
					LabeledValue->SetFontSizeValue(GetFontSizeValues());

					// Explicitly discard the return value to satisfy [[nodiscard]] requirement
					(void)ScrollableLabeledValue->GetScrollBox()->AddChild(LabeledValue);
				}
			}
		}
	}
}

void FInv_AbilitySystemModifier::Manifest()
{
	FInv_InventoryItemFragment::Manifest();
}

void FInv_AbilitySystemModifier::ApplyModifier(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;

	if (AbilitySystemData.GameplayEffectClass)
	{
		AbilitySystemData.ApplyEffectAndEffectCue(PlayerController);
	}
	
	if (AbilitySystemData.AbilityToGrant)
	{
		if (AbilitySystemData.GrantAbilityPermanently)
		{
			AbilitySystemData.ApplyAbilityAndAbilityCuePermanently(PlayerController);
			if ( AbilitySystemData.ActivateAbilityImmediately)
			{
				UInv_InventoryStatics::GetAbilitySystemComponent(PlayerController)->TryActivateAbility(AbilitySystemData.ActiveAbilityHandle);
			}
		}
		else
		{
			AbilitySystemData.ApplyAbilityAndAbilityCue(PlayerController);
		}
	}
	
}

void FInv_AbilitySystemModifier::RemoveModifier(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;
	if (AbilitySystemData.ActiveAbilityHandle.IsValid() && AbilitySystemData.AbilityToGrant)
	{
		AbilitySystemData.RemoveAbility(PlayerController);
	}

	if (AbilitySystemData.ActiveEffectHandle.IsValid() && AbilitySystemData.GameplayEffectClass)
	{
		AbilitySystemData.RemoveEffect(PlayerController);
	}
}

#pragma endregion

#pragma region Ability System Data
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

#pragma endregion

#pragma region Equippable Fragment

void FInv_EquippableFragment::Manifest()
{
	if (AbilityModifiers.Num() > 0)
	{
		bHasAbilityModifiers = true;
		for (FInv_AbilitySystemModifier& Modifer : AbilityModifiers)
		{
			Modifer.Manifest();
		}
	}

	FInv_InventoryItemFragment::Manifest();
}

void FInv_EquippableFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	for ( const FInv_AbilitySystemModifier& Modifer : AbilityModifiers)
	{
		Modifer.Assimilate(Composite);
	}
	FInv_InventoryItemFragment::Assimilate(Composite);
}

FInv_AbilitySystemModifier* FInv_EquippableFragment::FindModifer(int32 Index)
{
	if (AbilityModifiers.IsValidIndex(Index))
	{
		return &AbilityModifiers[Index];
	}

	return nullptr;
}

FInv_AbilitySystemModifier* FInv_EquippableFragment::FindModiferByTag(const FGameplayTag& Tag)
{
	if (AbilityModifiers.Num() == 0) return nullptr;
	
	for (auto& Data : AbilityModifiers )
	{
		FInv_AbilitySystemModifier* Modifer = &Data;
		if ( Data.AbilitySystemData.EffectTag == Tag )
		{
			return Modifer;
		}

		if ( Data.AbilitySystemData.AbilityTag == Tag )
		{
			return Modifer;
		}
	}

	return nullptr;
}

void FInv_EquippableFragment::OnEquip(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;

	if (AbilityModifiers.Num() == 0) return;
	
	for ( auto& Data : AbilityModifiers )
	{
		Data.ApplyModifier(PlayerController, Item);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Equipped"));
}

void FInv_EquippableFragment::OnUnequip(APlayerController* PlayerController, UInv_InventoryItem* Item)
{
	if (!IsValid(PlayerController)) return;

	if (AbilityModifiers.Num() == 0) return;
	
	for ( auto& Data : AbilityModifiers )
	{
		Data.RemoveModifier(PlayerController, Item);
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("UnEquipped"));
}

#pragma endregion
