// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/MM_CollectionInfoPopUpWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"

void UMM_CollectionInfoPopUpWidget::UpdateAndActivatePopUp(FPieceDataStruct PieceData)
{
	/* Update all of the displayed information. */
	PreviewImage->SetBrushFromTexture(PieceData.PieceInfoPhoto, false);
	PieceName->SetText(FText::FromString(PieceData.PieceName));

	/* Format the color of the rarity text. This method requires that the display name of piece
	 * rarities is also the name of its rich text style. */
	const FString RarityText = StaticEnum<EPieceRarity>()->GetValueAsString(PieceData.Rarity);
	const FString ClassText = StaticEnum<EPieceClass>()->GetValueAsString(PieceData.ClassType);
	const FString NewTypeText = "<" + RarityText + ">" + RarityText + " " + ClassText + "</>";
	PieceTypeText->SetText(FText::FromString(NewTypeText));

	PieceDescription->SetText(FText::FromString(PieceData.PieceDes));
	StrengthText->SetText(FText::FromString(FString::FromInt(PieceData.BaseStrength)));
	ArmorText->SetText(FText::FromString(FString::FromInt(PieceData.BaseArmor)));

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
}
