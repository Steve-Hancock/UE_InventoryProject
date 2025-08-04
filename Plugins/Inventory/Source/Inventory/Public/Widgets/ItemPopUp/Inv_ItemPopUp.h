// © 2025 Steven Hancock, operating under SigmaPrimus. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_ItemPopUp.generated.h"

class USizeBox;
class UTextBlock;
class USlider;
class UButton;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FPopUpMenuSplit, int32, SplitAmount, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuDrop, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuConsume, int32, Index);

/**
 * 
 */
UCLASS()
class INVENTORY_API UInv_ItemPopUp : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	FPopUpMenuSplit OnSplit;
	FPopUpMenuDrop OnDrop;
	FPopUpMenuConsume OnConsume;
	
	int32 GetSplitAmount() const;
	FVector2D GetBoxSize() const;
	int32 GetGridIndex() const {return GridIndex;}
	
	void SetSliderParameters(const float Max, const float Value) const;
	void SetGridIndex(const int32 Index) {GridIndex = Index;}

	void CollapseSplitButton() const;
	void CollapseConsumeButton() const;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Split;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Drop;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Consume;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_Split;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SplitAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	int32 GridIndex{INDEX_NONE};

	UFUNCTION()
	void SplitButtonClicked();

	UFUNCTION()
	void DropButtonClicked();

	UFUNCTION()
	void ConsumeButtonClicked();

	UFUNCTION()
	void SliderValueChanged(float Value);


	
};
