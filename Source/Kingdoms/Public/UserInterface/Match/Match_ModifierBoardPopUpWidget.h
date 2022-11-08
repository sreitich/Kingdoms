// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_ModifierBoardPopUpWidget.generated.h"

class UImage;
class UTextBlock;
class UTexture2D;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_ModifierBoardPopUpWidget : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Updates this widget's displayed information. */
	UFUNCTION(BlueprintCallable)
	void UpdateModifierWidget(int NetModifierValue, bool bStrength);


/* Protected variables. */
protected:

	/* The color of the pop-up text when the modifier increases a stat. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Colors")
	FLinearColor StatIncreaseColor;

	/* The color of the pop-up text when the modifier increases a stat. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Colors")
	FLinearColor StatDecreaseColor;


/* Protected assets. */
protected:

	/* Displayed for modifiers that affect a piece's strength. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Assets")
	UTexture2D* StrengthIcon;

	/* Displayed for modifiers that affect a piece's armor. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Assets")
	UTexture2D* ArmorIcon;

	
/* Protected widgets. */
protected:

	/* Text that displays the new modifier's value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTextBlock* ModifierValue;

	/* Image that displays either the armor or the strength icons to indicate the targeted stat. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UImage* StatImage;
};
