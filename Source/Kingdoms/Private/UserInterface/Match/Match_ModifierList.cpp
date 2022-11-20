// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/Match_ModifierList.h"

#include "UserInterface/Match/Match_Modifier.h"
#include "UserDefinedData/Match_UserDefinedData.h"

#include "Components/ScrollBox.h"
#include "UserInterface/Match/Match_PieceInfoWidget.h"

void UMatch_ModifierList::PopulateModifierList(UMatch_PieceInfoWidget* SpawningPieceInfoWidget, TArray<FModifier> Modifiers, bool bAlignedLeft)
{
	/* Remove any modifiers that were already listed. */
	for (UWidget* Widget : ModifierListBox->GetAllChildren())
	{
		Widget->RemoveFromParent();
	}
	
	/* Create a new modifier widget for every modifier given, update its displayed text, and add it to the modifier list box. */
	for (int i = 0; i < Modifiers.Num(); i++)
	{
		if (ModifierClass)
		{
			UMatch_Modifier* ModifierWidget = Cast<UMatch_Modifier>(CreateWidget<UUserWidget>(GetWorld(), ModifierClass, FName("Modifier " + i)));
			ModifierListBox->AddChild(ModifierWidget);
			/* Align the widget's text to the left depending on whether or not the piece is friendly. */
			ModifierWidget->UpdateDisplayedModifier(this, Modifiers[i], bAlignedLeft);
		}
	}

	/* Save a pointer to the piece info widget that spawned this widget. */
	if (SpawningPieceInfoWidget)
	{
		PieceInfoWidget = SpawningPieceInfoWidget;
	}
}

void UMatch_ModifierList::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

    /* Destroy the modifier list if the player is not hovering over the piece's stats. */
	if (!PieceInfoWidget->AreStatsHovered())
	{
		PieceInfoWidget->ModifierList = nullptr;
		RemoveFromParent();
	}
}
