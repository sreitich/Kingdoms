// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_ArmyPresetPreview.h"

#include "Components/Button.h"
#include "Framework/MainMenu/MM_PlayerController.h"

void UMM_ArmyPresetPreview::NativeConstruct()
{
	Super::NativeConstruct();

	SelectPresetButton->OnClicked.AddDynamic(this, &UMM_ArmyPresetPreview::OnPresetButtonClicked);
}

void UMM_ArmyPresetPreview::UpdateArmyPresetPreview(uint8 NewPresetSlot)
{
	PresetSlot = NewPresetSlot;

	// Navigate to army preset builder
}

void UMM_ArmyPresetPreview::OnPresetButtonClicked()
{
	/* Set this preset as the selected one. */
	GetOwningPlayer<AMM_PlayerController>()->SelectedArmyPreset = PresetSlot;
}
