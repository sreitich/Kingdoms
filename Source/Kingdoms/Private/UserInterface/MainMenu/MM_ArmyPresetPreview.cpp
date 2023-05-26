// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_ArmyPresetPreview.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Framework/KingdomsGameInstance.h"
#include "Framework/MainMenu/MM_PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGames/ArmyPresets_SaveGame.h"

void UMM_ArmyPresetPreview::NativeConstruct()
{
	Super::NativeConstruct();

	SelectPresetButton->OnClicked.AddDynamic(this, &UMM_ArmyPresetPreview::OnPresetButtonClicked);
}

void UMM_ArmyPresetPreview::UpdateArmyPresetPreview(uint8 NewPresetSlot)
{
	PresetSlot = NewPresetSlot;

	const UKingdomsGameInstance* GameInstance = Cast<UKingdomsGameInstance>(UGameplayStatics::GetGameInstance(GetOwningPlayer()));
	const FArmyPresetStruct ArmyPreset = GameInstance->ArmyPresets_SaveGame->GetArmyPresets()[NewPresetSlot];

	// Update the preset name.
	PresetName->SetText(FText::FromString(ArmyPreset.ArmyName));

	// Update the preset preview images.
	if (IsValid(PieceDataTable))
	{
		static const FString ContextString(TEXT("Piece Data Struct"));

		Piece1Preview->SetBrushFromTexture(PieceDataTable->FindRow<FPieceDataStruct>(FName(ArmyPreset.Pieces[0]), ContextString, true)->PieceCardPortrait);
		Piece2Preview->SetBrushFromTexture(PieceDataTable->FindRow<FPieceDataStruct>(FName(ArmyPreset.Pieces[1]), ContextString, true)->PieceCardPortrait);
		Piece3Preview->SetBrushFromTexture(PieceDataTable->FindRow<FPieceDataStruct>(FName(ArmyPreset.Pieces[2]), ContextString, true)->PieceCardPortrait);
	}
}

void UMM_ArmyPresetPreview::OnPresetButtonClicked()
{
	/* Set this preset as the selected one. */
	GetOwningPlayer<AMM_PlayerController>()->SelectedArmyPreset = PresetSlot;
	
	// Navigate to army preset builder
}
