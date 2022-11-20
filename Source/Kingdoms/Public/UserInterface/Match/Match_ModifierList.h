// Copyright Samuel Reitich 2023.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Match_ModifierList.generated.h"

class UOverlay;
class UScrollBox;

class UMatch_Modifier;
class UMatch_PieceInfoWidget;

struct FModifier;

/**
 * 
 */
UCLASS()
class KINGDOMS_API UMatch_ModifierList : public UUserWidget
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Populates this list with a given list of modifier widgets and updates each widget's text to display its modifier info. */
	UFUNCTION(BlueprintCallable)
	void PopulateModifierList(UMatch_PieceInfoWidget* SpawningPieceInfoWidget, TArray<FModifier> Modifiers, bool bAlignedLeft);

	/* Destroys this widget if the player stops hovering over it and is also not hovering over the piece info widget. */
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	/* Returns the piece info widget that created this modifier list widget. */
	UFUNCTION()
	FORCEINLINE UMatch_PieceInfoWidget* GetSpawningPieceInfoWidget() const { return PieceInfoWidget; }
	

/* Public widgets. */
public:

	/* The parent overlay of this widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	UOverlay* ModifierListOverlay;


/* Protected variables. */
protected:

	/* The piece info widget that created this modifier list widget. */
	UPROPERTY()
	UMatch_PieceInfoWidget* PieceInfoWidget;


/* Protected widgets. */
protected:

	/* The box that modifiers are added to. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	UScrollBox* ModifierListBox;


/* Protected widget class-types. */
protected:

	/* Modifier class with which to populate this list. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget Class-Types")
	TSubclassOf<UMatch_Modifier> ModifierClass;

};
