// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/Match_Modifier.h"

#include "Board/BoardTile.h"
#include "Components/Button.h"
#include "Components/RichTextBlock.h"
#include "Pieces/ParentPiece.h"
#include "UserDefinedData/Match_UserDefinedData.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

void UMatch_Modifier::UpdateDisplayedModifier(FModifier NewModifier, bool bAlignedLeft)
{
	/* Save the information of the modifier that this widget represents. */
	ModifierInfo = NewModifier;
	/* The string that this modifier pop-up will display. */
	FString NewModifierText;

	/* Modifier value. */
	if (ModifierInfo.Value > 0)
	{
		NewModifierText = "<Buff>+" + FString::FromInt(ModifierInfo.Value) + "</>";
	}
	else if (ModifierInfo.Value < 0)
	{
		NewModifierText = "<Debuff>" + FString::FromInt(ModifierInfo.Value) + "</>";
	}
	else
	{
		NewModifierText = "<NoEffect>+0</>";
	}


	NewModifierText += " from ";


	/* If the piece is dead. */
	if (!IsValid(ModifierInfo.SourceActor))
	{
		NewModifierText += "<Dead>eliminated</> ";
	}
	

	/* Source's alignment and piece's name. */
	if (ModifierInfo.SourceAlignmentToTarget == E_Neutral)
	{
		NewModifierText += "<NeutralName>";
	}
	else if (ModifierInfo.SourceActor->GetInstigator()->IsLocallyControlled())
	{
		NewModifierText += "<FriendlyName>friendly ";
	}
	else
	{
		NewModifierText += "<EnemyName>enemy ";
	}

	NewModifierText += ModifierInfo.SourceName + "'s</> ";


	/* Ability name. */
	NewModifierText += "<AbilityName>" + ModifierInfo.SourceAbilityName + "</> ";


	/* Remaining duration. */
	if (NewModifier.RemainingDuration > 1)
	{
		NewModifierText += "<Duration>(</><DurationCount>" + FString::FromInt(NewModifier.RemainingDuration) + "</><Duration> turns remaining)</>";
	}
	else
	{
		NewModifierText += "<Duration>(</><DurationCount>1</><Duration> turn remaining)</>";
	}

	/* Update the displayed text with the built string of modifier info. */
	DisplayedModifierInfo->SetText(FText::FromString(NewModifierText));

	/* Align the text to the left or right depending on if the displayed piece is friendly or an enemy. */
	if (bAlignedLeft)
	{
		DisplayedModifierInfo->SetJustification(ETextJustify::Left);
	}
	else
	{
		DisplayedModifierInfo->SetJustification(ETextJustify::Right);
	}
}

void UMatch_Modifier::NativeConstruct()
{
	Super::NativeConstruct();

	ModifierButton->OnHovered.AddDynamic(this, &UMatch_Modifier::OnModifierButtonHovered);
	ModifierButton->OnUnhovered.AddDynamic(this, &UMatch_Modifier::OnModifierButtonUnhovered);
}

void UMatch_Modifier::NativeDestruct()
{
	Super::NativeDestruct();

	/* Remove the source's highlight. */
	HighlightSource(true);
}

void UMatch_Modifier::OnModifierButtonHovered()
{
	/* Highlight the source. */
	HighlightSource(false);
}

void UMatch_Modifier::OnModifierButtonUnhovered()
{
	/* Remove the source's highlight. */
	HighlightSource(true);
}

void UMatch_Modifier::HighlightSource(bool bRemoveHighlight) const
{
	/* If the actor responsible for this modifier is a piece, highlight it or remove the highlight. */
	if (AParentPiece* SourcePiece = Cast<AParentPiece>(ModifierInfo.SourceActor))
	{
		SourcePiece->FlashHighlight
		(
			SourcePiece->GetAlignment() == E_Friendly ? SourcePiece->FriendlyFresnelColor : SourcePiece->EnemyFresnelColor,
			bRemoveHighlight ? 2.0f : 20.0f,
			0.5f,
			0.0f,
			true
		);
	}
	/* If the actor responsible for this modifier is a tile, highlight it or remove the highlight. */
	else if (ABoardTile* SourceTile = Cast<ABoardTile>(ModifierInfo.SourceActor))
	{
		SourceTile->UpdateEmissiveHighlight(!bRemoveHighlight, 1.0f, SourceTile->Highlight_ValidUnoccupiedTile);
	}
}
