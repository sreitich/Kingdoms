// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_ArmyMenuWidget.h"

#include "UserInterface/MainMenu/MM_HUD.h"

#include "Components/Button.h"
#include "Components/ButtonSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Framework/KingdomsGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGames/ArmyPresets_SaveGame.h"
#include "UserDefinedData/SaveGameData_UserDefinedData.h"
#include "UserInterface/MainMenu/MM_ArmyPresetPreview.h"

void UMM_ArmyMenuWidget::DeactivateWidget()
{
	/* Play the deactivation animation, which destroys this widget and transitions the menu after it finishes. */
	PlayAnimationForward(OnDeactivatedAnim, 1.0f, false);
}

void UMM_ArmyMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();



	TArray<FArmyPresetStruct> ArmyPresets = Cast<UKingdomsGameInstance>(UGameplayStatics::GetGameInstance(GetOwningPlayer()))->ArmyPresets_SaveGame->GetArmyPresets();
	TArray<FArmyPresetStruct> ValidArmyPresets = TArray<FArmyPresetStruct>();

	// Get all of the army presets that have been created (so they have at least one piece).
	for (FArmyPresetStruct Preset : ArmyPresets)
	{
		if (Preset.Pieces.Num() > 0)
		{
			ValidArmyPresets.Add(Preset);
		}
	}

	// For every army preset that has been created, add a preview that can be used to navigate to it.
	for (int PresetIndex = 0; PresetIndex < ValidArmyPresets.Num(); PresetIndex++)
	{
		UMM_ArmyPresetPreview* ArmyPresetPreviewWidget = CreateWidget<UMM_ArmyPresetPreview>(GetOwningPlayer(), ArmyPresetClass, FName(ValidArmyPresets[PresetIndex].ArmyName + " Army Preset Widget"));
		if (ArmyPresetPreviewWidget)
		{
			ArmyPresetPreviewWidget->UpdateArmyPresetPreview(PresetIndex);
			ArmyPresetBox->AddChild(ArmyPresetPreviewWidget);
		}
	}

	// If the player still has army preset slots left, add a button to allow them to make a new one.
	const bool bHasSlots = ValidArmyPresets.Num() < MaxPresets;
	NewPresetButton->SetIsEnabled(bHasSlots);
	NewPresetButton->SetVisibility(bHasSlots ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	ArmyPresetBox->AddChild(NewPresetButton);
	UHorizontalBoxSlot* PresetButtonSlot = CastChecked<UHorizontalBoxSlot>(NewPresetButton->Slot);
	if (PresetButtonSlot)
	{
		PresetButtonSlot->SetPadding(FMargin(15.0f, 0.0f));
	}



	/* Bind the "collection" button to navigate to the collection menu. */
	CollectionButton->OnClicked.AddDynamic(this, &UMM_ArmyMenuWidget::OnCollectionClicked);
	/* Bind the "back" button to deactivate this widget. */
	BackButton->OnClicked.AddDynamic(this, &UMM_ArmyMenuWidget::OnBackClicked);

	/* Play the activation animation to reveal this widget. */
	PlayAnimationForward(OnActivatedAnim, 1.0f, false);

	/* Bind the deactivation animation to destroy this widget when it finishes playing. */
	FWidgetAnimationDynamicEvent AnimBinding = FWidgetAnimationDynamicEvent();
	AnimBinding.BindDynamic(this, &UMM_ArmyMenuWidget::OnDeactivatedAnimEnd);
	BindToAnimationFinished(OnDeactivatedAnim, AnimBinding);
}

void UMM_ArmyMenuWidget::OnDeactivatedAnimEnd()
{
	/* When this widget is deactivated, safely destroy it and transition to the next queued menu in the HUD. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->ChangeMenus();
	HUDPtr->CreateArmyMenuWidget(true);
}

void UMM_ArmyMenuWidget::OnCollectionClicked()
{
	/* Queues the collection menu, which will deactivate this menu and transition to the new one. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->QueueMenuChange(E_CollectionMenu);
}

void UMM_ArmyMenuWidget::OnBackClicked()
{
	/* To navigate back to the main menu, queue the menu, which will deactivate this menu and transition to the new
	 * one. */
	AMM_HUD* HUDPtr = GetOwningPlayer()->GetHUD<AMM_HUD>();
	HUDPtr->QueueMenuChange(E_MainMenu);
}
