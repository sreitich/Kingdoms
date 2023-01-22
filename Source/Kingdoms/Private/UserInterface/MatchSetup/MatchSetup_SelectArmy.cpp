// Copyright Change Studios, LLC 2023.


#include "UserInterface/MatchSetup/MatchSetup_SelectArmy.h"

#include "Framework/KingdomsGameInstance.h"
#include "UserDefinedData/SaveGameData_UserDefinedData.h"
#include "UserInterface/MatchSetup/MatchSetup_ArmyPresetWidget.h"
#include "Framework/Match/Match_PlayerController.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/UMG/Public/UMG.h"
#include "SaveGames/ArmyPresets_SaveGame.h"

void UMatchSetup_SelectArmy::NativeConstruct()
{
	Super::NativeConstruct();
	
	/* Get the game instance. */
	const UKingdomsGameInstance* KingdomsGameInstance = Cast<UKingdomsGameInstance>(GetGameInstance());

	/* If the game instance was found... */
	if (KingdomsGameInstance)
	{
		/* For every army preset... */
		for (int i = 0; i < 5; i++)
		{
			/* If the army preset widget class has been set and if this army preset has been created by the user (i.e. it has pieces in it)... */
			if (IsValid(MatchSetup_ArmyPresetWidgetClass) && KingdomsGameInstance->ArmyPresets_SaveGame->ArmyPresets[i].Pieces.Num() >  0)
			{
				/* Create an army preset widget for this army preset. */
				UMatchSetup_ArmyPresetWidget* ArmyPresetWidget = CreateWidget<UMatchSetup_ArmyPresetWidget>(GetOwningPlayer(), MatchSetup_ArmyPresetWidgetClass, FName(KingdomsGameInstance->ArmyPresets_SaveGame->ArmyPresets[i].ArmyName));

				/* If the widget was successfully created... */
				if (ArmyPresetWidget)
				{
					/* Add the army preset widget to the army preset list container. */
					ArmyListBox->AddChild(ArmyPresetWidget);

					ArmyPresetWidget->ParentWidget = this;
					
					/* Tell the army preset widget which army preset it represents. */
					ArmyPresetWidget->ArmyPreset = KingdomsGameInstance->ArmyPresets_SaveGame->ArmyPresets[i];
					
					/* Update the army preset widget's information. */
					ArmyPresetWidget->UpdateArmyPresetWidget();

					/* Add padding between each widget. */
					ArmyPresetWidget->SetPadding(FMargin(15.0f, 0.0f));
				}
			}
		}		
	}
	
    /* Bind the ready button to confirm the army and allow the player to start placing pieces. */
    SelectArmyButton->OnClicked.AddDynamic(this, &UMatchSetup_SelectArmy::OnSelectArmyClicked);
}

void UMatchSetup_SelectArmy::OnSelectArmyClicked()
{
    /* Create the place pieces widget. */
    GetOwningPlayer<AMatch_PlayerController>()->CreatePlacePiecesWidget(SelectedArmy, false);

    /* Destroy this widget. */
    RemoveFromParent();
}