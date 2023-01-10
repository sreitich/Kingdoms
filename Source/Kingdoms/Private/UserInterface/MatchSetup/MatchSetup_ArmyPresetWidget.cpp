// Copyright Change Studios, LLC 2023.


#include "UserInterface/MatchSetup/MatchSetup_ArmyPresetWidget.h"

#include "Components/TextBlock.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UserInterface/MatchSetup/MatchSetup_SelectArmy.h"

void UMatchSetup_ArmyPresetWidget::UpdateArmyPresetWidget()
{
	/* Update the displayed army preset name. */
	ArmyName->SetText(FText::FromString(ArmyPreset.ArmyName));
	
	/* Add later: update the displayed army based on the army's piece composition. */
}

void UMatchSetup_ArmyPresetWidget::NativeConstruct()
{
	Super::NativeConstruct();

	/* Bind the select button to select the army. */
	SelectButton->OnClicked.AddDynamic(this, &UMatchSetup_ArmyPresetWidget::OnSelectClicked);
}

void UMatchSetup_ArmyPresetWidget::OnSelectClicked()
{
	TArray<UUserWidget*> AllArmyPresetWidgets;

	/* If the army preset widget blueprint class has been set... */
	if (IsValid(ParentWidget->MatchSetup_ArmyPresetWidgetClass))
	{
		/* Get every army preset widget. */
		UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), AllArmyPresetWidgets, ParentWidget->MatchSetup_ArmyPresetWidgetClass, false);
	}

	/* For every army preset widget... */
	for (UUserWidget* ArmyPresetWidget : AllArmyPresetWidgets)
	{
		/* Indicate that other armies have been deselected. */
		Cast<UMatchSetup_ArmyPresetWidget>(ArmyPresetWidget)->SelectButton->SetBackgroundColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
	}
	
	/* Indicate that the new army has been selected. */
	SelectButton->SetBackgroundColor(FLinearColor(0.0f, 1.0f, 1.0f, 1.0f));

	/* Set the currently selected army in the SelectArmy widget. */
	ParentWidget->SelectedArmy = ArmyPreset;

	/* Allow the player to confirm their army selection if this is the first time they've selected an army. */
	if (!ParentWidget->SelectArmyButton->GetIsEnabled())
	{
		ParentWidget->SelectArmyButton->SetIsEnabled(true);
	}
}