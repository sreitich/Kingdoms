// Copyright Samuel Reitich 2023.


#include "UserInterface/Match/Match_AbilityInfoPopup.h"

#include "Pieces/ParentPiece.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "Components/TextBlock.h"

void UMatch_AbilityInfoPopup::SetUpWidget(AParentPiece* DisplayedPiece, bool bActive, bool bAlignedLeft)
{
	/* Piece's data table asset pointers are public, so we can just steal one from the piece whose ability is being displayed. */
	if (DisplayedPiece->PieceDataTable)
	{
		/* Get this piece's row from the piece data. */
		static const FString ContextString(TEXT("Piece Data Struct"));
		const FPieceDataStruct* PieceData = DisplayedPiece->PieceDataTable->FindRow<FPieceDataStruct>(DisplayedPiece->PieceID, ContextString, true);

		/* If the data table row was found... */
		if (PieceData)
		{
			/* Update the displayed ability name and description with the piece data, depending on whether the active
			 * or passive ability is being displayed. */
			DisplayedAbilityName->SetText(FText::FromString(bActive ? PieceData->ActiveName : PieceData->PassiveName));
			DisplayedAbilityDescription->SetText(FText::FromString(bActive ? PieceData->ActiveDes : PieceData->PassiveDes));
		}
	}

	/* Set the justification of the text. */
	if (bAlignedLeft)
	{
		DisplayedAbilityName->SetJustification(ETextJustify::Left);
		DisplayedAbilityDescription->SetJustification(ETextJustify::Left);
	}
	else
	{
		DisplayedAbilityName->SetJustification(ETextJustify::Right);
		DisplayedAbilityDescription->SetJustification(ETextJustify::Right);
	}
}

void UMatch_AbilityInfoPopup::NativeConstruct()
{
	Super::NativeConstruct();
}
