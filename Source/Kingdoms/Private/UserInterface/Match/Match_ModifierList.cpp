// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/Match_ModifierList.h"

#include "UserInterface/Match/Match_Modifier.h"
#include "UserDefinedData/Match_UserDefinedData.h"

#include "Components/ScrollBox.h"

void UMatch_ModifierList::PopulateModifierList(TArray<FModifier> Modifiers)
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
			ModifierWidget->UpdateDisplayedModifier(Modifiers[i]);
		}
	}
}
