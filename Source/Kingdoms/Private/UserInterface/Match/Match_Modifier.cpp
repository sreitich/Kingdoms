// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/Match_Modifier.h"

#include "Components/RichTextBlock.h"
#include "Pieces/ParentPiece.h"
#include "UserDefinedData/Match_UserDefinedData.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

void UMatch_Modifier::UpdateDisplayedModifier(FModifier NewModifier, bool bAlignedLeft)
{
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
			ModifierInfo->SetText(FText::FromString(NewModifierText));
		}
	}

	/* Align the text to the left or right depending on if the displayed piece is friendly or an enemy. */
	if (bAlignedLeft)
	{
		ModifierInfo->SetJustification(ETextJustify::Left);
	}
	else
	{
		ModifierInfo->SetJustification(ETextJustify::Right);
	}
}

void UMatch_Modifier::NativeConstruct()
{
	Super::NativeConstruct();
}
