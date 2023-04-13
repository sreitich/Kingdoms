// Copyright Change Studios, LLC 2023.


#include "UserInterface/MainMenu/PieceCollectionListing.h"

#include "Components/Image.h"
#include "Pieces/ParentPiece.h"

#include "Components/TextBlock.h"

void UPieceCollectionListing::UpdatePieceCollectionListing(FPieceDataStruct NewPieceData, bool bOwned)
{
	/* Ensure that the piece to update to exists and is not already represented by this widget. */
	if (NewPieceData.PieceName.Len() && RepresentedPieceName != NewPieceData.PieceName)
	{
		/* Save the new piece's name. */
		RepresentedPieceName = NewPieceData.PieceName;

		/* Update the piece's displayed image. */
		PreviewImage->SetBrushFromTexture(NewPieceData.PieceCardPortrait, false);

		/* Update the piece's displayed name. */
		Name->SetText(FText::FromString(NewPieceData.PieceName));


		/* Enable or disable the cover indicating that the piece is unowned depending on the player's piece ownership. */
		UnownedCover->SetVisibility(bOwned ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible);
	}
}
