// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/Match_Modifier.h"

#include "Board/BoardTile.h"
#include "Pieces/ParentPiece.h"
#include "UserDefinedData/Match_UserDefinedData.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"
#include "UserInterface/Match/Match_ModifierList.h"
#include "UserInterface/Match/Match_PieceInfoWidget.h"

#include "Components/Button.h"
#include "Components/RichTextBlock.h"


void UMatch_Modifier::UpdateDisplayedModifier(UMatch_ModifierList* InParentModifierList, FModifier NewModifier, bool bAlignedLeft)
{
	/* Save this modifier widget's parent modifier list widget. */
	ParentModifierList = InParentModifierList;
	/* Save the information of the modifier that this widget represents. */
	ModifierInfo = NewModifier;
	/* The string that this modifier pop-up will display. */
	FString NewModifierText = "";

	/* Modifier strength value. */
	if (ModifierInfo.StrengthChange != 0)
	{
		// Strength buff.
		if (ModifierInfo.StrengthChange > 0)
			NewModifierText += "<Buff>+" + FString::FromInt(ModifierInfo.StrengthChange) + "S</>";
		// Strength debuff.
		else
			NewModifierText += "<Debuff>" + FString::FromInt(ModifierInfo.StrengthChange) + "S</>";
	}


	/* Add an "and" if the modifier affects both stats. */
	if (ModifierInfo.StrengthChange != 0 && ModifierInfo.ArmorChange != 0)
		NewModifierText += " and ";


	if (ModifierInfo.ArmorChange != 0)
	{
		// Armor buff.
		if (ModifierInfo.ArmorChange > 0)
			NewModifierText += "<Buff>+" + FString::FromInt(ModifierInfo.ArmorChange) + "A</>";
		// Armor debuff.
		else
			NewModifierText += "<Debuff>" + FString::FromInt(ModifierInfo.ArmorChange) + "A</>";
	}


	/* Display active modifiers even if they don't have any effects. */
	if (ModifierInfo.StrengthChange == 0 && ModifierInfo.ArmorChange == 0)
	{
		NewModifierText += "<NoEffect>+0S</> and <NoEffect>+0A</>";
	}


	NewModifierText += " from ";


	/* If the piece is dead. */
	if (!IsValid(ModifierInfo.SourceActor))
	{
		NewModifierText += "<Dead>eliminated</> ";
	}


	/* Source's alignment and name. */
	if (ModifierInfo.SourceAlignmentToTarget == E_Neutral)
	{
		NewModifierText += "<NeutralName>" + ModifierInfo.SourceName + "'s</> ";
	}
	else if (ModifierInfo.SourceActor->GetInstigator()->IsLocallyControlled())
	{
		NewModifierText += "<FriendlyPrefix>friendly</> <FriendlyName>" + ModifierInfo.SourceName + "'s</> ";
	}
	else
	{
		NewModifierText += "<EnemyPrefix>enemy</> <EnemyName>" + ModifierInfo.SourceName + "'s</> ";
	}


	/* Ability name. */
	NewModifierText += "<AbilityName>" + ModifierInfo.SourceAbilityName + "</> ";


	/* Remaining duration. Only display if piece has a definite duration. */
	if (!NewModifier.bIndefiniteDuration)
	{
		if (NewModifier.RemainingDuration > 1)
		{
			NewModifierText += "<Duration>(</><DurationCount>" + FString::FromInt(NewModifier.RemainingDuration) + "</><Duration> turns remaining)</>";
		}
		else
		{
			NewModifierText += "<Duration>(</><DurationCount>1</><Duration> turn remaining)</>";
		}
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
	/* Remove the source's highlight unless the source is the currently selected piece. */
	HighlightSource(true);

	/* Unbind the button's delegates. */
	ModifierButton->OnHovered.RemoveAll(this);
	ModifierButton->OnUnhovered.RemoveAll(this);

	Super::NativeDestruct();
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
		/* Set the brightness that the piece's highlight will be set to. */
		float Brightness = 0.0f;

		/* If the piece is being highlighted. */
		if (!bRemoveHighlight)
			Brightness = SourcePiece->FlashFresnelStrength * 1.5f;
		/* If the piece's highlight is being removed. */
		else
			/* If the piece is currently selected by the player. */
			if (ParentModifierList->GetSpawningPieceInfoWidget()->GetDisplayedPiece() == ModifierInfo.SourceActor)
				Brightness = SourcePiece->SelectedFresnelStrength;
			/* If the piece is not currently selected by the player. */
			else
				Brightness = SourcePiece->DefaultFresnelStrength;

		/* Set the piece's requested highlight. */
		SourcePiece->FlashHighlight
		(
			SourcePiece->GetAlignment() == E_Friendly ? SourcePiece->FriendlyFresnelColor : SourcePiece->EnemyFresnelColor,
			Brightness,
			HighlightSourceRate,
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
