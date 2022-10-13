// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/Match_Modifier.h"

#include "Components/Button.h"
#include "Components/RichTextBlock.h"
#include "Pieces/ParentPiece.h"
#include "UserDefinedData/Match_UserDefinedData.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

void UMatch_Modifier::UpdateDisplayedModifier(FModifier NewModifier, bool bAlignedLeft)
{
	/* Save the information of the modifier that this widget represents. */
	ModifierInfo = NewModifier;

	if (PieceDataTable)
	{
		/* Get this source piece's row from the piece data table. */
		static const FString ContextString(TEXT("Piece Data Struct"));
		const FPieceDataStruct* PieceData = PieceDataTable->FindRow<FPieceDataStruct>(NewModifier.SourcePiece->PieceID, ContextString, true);

		/* If the data table row was found, update this modifier's displayed info. */
		if (PieceData)
		{
			FString NewModifierText = "";

			/* Modifier value. */
			if (NewModifier.Value > 0)
			{
				NewModifierText = "<Buff>+" + FString::FromInt(NewModifier.Value) + "</>";
			}
			else if (NewModifier.Value < 0)
			{
				NewModifierText = "<Debuff>" + FString::FromInt(NewModifier.Value) + "</>";
			}
			else
			{
				NewModifierText = "<NoEffect>+0</>";
			}

			NewModifierText += " from ";

			/* Piece's name. */
			if (NewModifier.SourcePiece->GetInstigator()->IsLocallyControlled())
			{
				NewModifierText += "<FriendlyName>friendly " + PieceData->PieceName + "'s </>";
			}
			else
			{
				NewModifierText += "<EnemyName>enemy " + PieceData->PieceName + "'s </>";
			}

			/* Ability name. */
			NewModifierText += "<AbilityName>" + NewModifier.SourceAbilityName + " </>";

			/* Remaining duration. */
			if (NewModifier.RemainingDuration > 1)
			{
				NewModifierText += "<Duration>(</><DurationCount>" + FString::FromInt(NewModifier.RemainingDuration) + "</><Duration> turns remaining)</>";
			}
			else
			{
				NewModifierText += "<Duration>(</><DurationCount>1</><Duration> turn remaining)</>";
			}

			/* Update the displayed text. */
			DisplayedModifierInfo->SetText(FText::FromString(NewModifierText));
		}
	}

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

	ModifierInfo.SourcePiece->FlashHighlight
	(
		ModifierInfo.SourcePiece->GetInstigator()->IsLocallyControlled() ? ModifierInfo.SourcePiece->FriendlyFresnelColor : ModifierInfo.SourcePiece->EnemyFresnelColor,
		2.0f,
		0.5f,
		0.0f,
		true
	);
}

void UMatch_Modifier::OnModifierButtonHovered()
{
	ModifierInfo.SourcePiece->FlashHighlight
		(
			ModifierInfo.SourcePiece->GetInstigator()->IsLocallyControlled() ? ModifierInfo.SourcePiece->FriendlyFresnelColor : ModifierInfo.SourcePiece->EnemyFresnelColor,
			20.0f,
			0.5f,
			0.0f,
			true
		);
}

void UMatch_Modifier::OnModifierButtonUnhovered()
{
	ModifierInfo.SourcePiece->FlashHighlight
	(
		ModifierInfo.SourcePiece->GetInstigator()->IsLocallyControlled() ? ModifierInfo.SourcePiece->FriendlyFresnelColor : ModifierInfo.SourcePiece->EnemyFresnelColor,
		2.0f,
		0.5f,
		0.0f,
		true
	);
}
