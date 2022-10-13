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

	UFUNCTION(BlueprintCallable)
	void PopulateModifierList(UMatch_PieceInfoWidget* SpawningPieceInfoWidget, TArray<FModifier> Modifiers, bool bAlignedLeft);

	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	

/* Public widgets. */
public:

	/* The parent overlay of this widget. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	UOverlay* ModifierListOverlay;


/* Protected variables. */
protected:

	UPROPERTY()
	UMatch_PieceInfoWidget* PieceInfoWidget;


/* Protected widgets. */
protected:

	/* The box that modifiers are added to. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	UScrollBox* ModifierListBox;


/* Protected widget class-types. */
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget Class-Types")
	TSubclassOf<UMatch_Modifier> ModifierClass;

};
