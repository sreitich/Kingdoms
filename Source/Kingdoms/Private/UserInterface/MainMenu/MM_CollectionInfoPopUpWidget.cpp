// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_CollectionInfoPopUpWidget.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "UserInterface/MainMenu/MM_HUD.h"

void UMM_CollectionInfoPopUpWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CloseButton->OnClicked.AddDynamic(this, &UMM_CollectionInfoPopUpWidget::ClosePopUp);
	BackgroundButton->OnClicked.AddDynamic(this, &UMM_CollectionInfoPopUpWidget::ClosePopUp);
}

void UMM_CollectionInfoPopUpWidget::UpdateAndActivatePopUp(FPieceDataStruct PieceData)
{
	/* Update all of the displayed information. */
	PreviewImage->SetBrushFromTexture(PieceData.PieceInfoPhoto, false);
	PieceName->SetText(FText::FromString(PieceData.PieceName));

	/* Format the color of the rarity text. This method requires that the display name of piece
	 * rarities is also the name of its rich text style. */
	FString RarityText = StaticEnum<EPieceRarity>()->GetValueAsString(PieceData.Rarity);
	RarityText.RemoveFromStart("E_");

	FString ClassText = StaticEnum<EPieceClass>()->GetValueAsString(PieceData.ClassType);
	ClassText.RemoveFromStart("E_");

	const FString NewTypeText = "<" + RarityText + ">" + RarityText + " " + ClassText + "</>";
	PieceTypeText->SetText(FText::FromString(NewTypeText));

	PieceDescription->SetText(FText::FromString("<Default>" + PieceData.PieceDes + "</>"));
	StrengthText->SetText(FText::FromString("<Stats>" + FString::FromInt(PieceData.BaseStrength) + "</>"));
	ArmorText->SetText(FText::FromString("<Stats>" + FString::FromInt(PieceData.BaseArmor) + "</>"));

	PassiveAbilityIcon->SetBrushFromTexture(PieceData.PassiveIcon, false);
	ActiveAbilityIcon->SetBrushFromTexture(PieceData.ActiveIcon, false);

	const FString PassiveText = "<AbilityName>" + PieceData.PassiveName + ": </><Default>" + PieceData.PassiveDes + "</>";
	PassiveAbilityText->SetText(FText::FromString(PassiveText));

	const FString ActiveText = "<AbilityName>" + PieceData.ActiveName + ": </><Default>" + PieceData.ActiveDes + "</>";
	ActiveAbilityText->SetText(FText::FromString(ActiveText));

	const bool bHasPassive = PieceData.PassiveName.Len() > 0;
	PassiveAbilityIcon->SetVisibility(bHasPassive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	PassiveAbilityText->SetVisibility(bHasPassive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	const bool bHasActive = PieceData.ActiveName.Len() > 0;
	ActiveAbilityIcon->SetVisibility(bHasActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	ActiveAbilityText->SetVisibility(bHasActive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	// If there is an active ability, move it depending on whether or not there is also a passive ability.
	if (PieceData.ActiveName.Len() > 0)
	{
		// If there is not a passive ability, swap the passive ability to the bottom and the active ability to the top.
		if (PieceData.PassiveName.Len() == 0)
		{
			TopAbilityWrapper->AddChild(ActiveBox);
			BottomAbilityWrapper->AddChild(PassiveBox);

			return;
		}
	}

	// If there isn't an active ability, or there's an active AND a passive ability, place the passive ability above the active ability.
	TopAbilityWrapper->AddChild(PassiveBox);
	BottomAbilityWrapper->AddChild(ActiveBox);
}

void UMM_CollectionInfoPopUpWidget::ClosePopUp()
{
	// Safely destroy this widget.
	GetOwningPlayer()->GetHUD<AMM_HUD>()->CreateCollectionPieceInfoPopUp(true, FPieceDataStruct());
}
