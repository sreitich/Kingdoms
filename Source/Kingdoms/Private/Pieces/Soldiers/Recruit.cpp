// Copyright Samuel Reitich 2023.


#include "Pieces/Soldiers/Recruit.h"

#include "Board/BoardTile.h"
#include "Pieces/PieceAIController.h"

ARecruit::ARecruit()
{
}

bool ARecruit::TileIsInMoveRange(ABoardTile* Tile)
{
	/* Make sure that a valid tile was passed. */
	if (IsValid(Tile))
	{
		/* Store the given tile and current tile's coordinates in variables for readability. */
		const int NewX = Tile->Coordinates.X, NewY = Tile->Coordinates.Y;
		const int OldX = CurrentTile->Coordinates.X, OldY = CurrentTile->Coordinates.Y;
	
		/* Test if the tile's coordinates match with any of this piece's move patterns. */
		if
		(
			/* Forward 1 */
			(NewX == OldX && NewY == OldY + 1) ||
			/* Right 1 */
			(NewX == OldX + 1 && NewY == OldY) ||
			/* Backward 1 */
			(NewX == OldX && NewY == OldY - 1) ||
			/* Left 1 */
			(NewX == OldX - 1 && NewY == OldY)
		)
		{
			return true;
		}
	}

	return false;
}

void ARecruit::BeginPlay()
{
	Super::BeginPlay();

}

void ARecruit::OnMoveToTileCompleted()
{
	Super::OnMoveToTileCompleted();

	
}