// Copyright Samuel Reitich 2023.


#include "Pieces/Paladins/Guardian.h"

#include "Board/BoardTile.h"

bool AGuardian::TileIsInMoveRange(ABoardTile* Tile)
{
	/* Make sure that a valid tile was passed. */
	if (IsValid(Tile))
	{
		/* Store the given tile and current tile's coordinates in variables for readability. */
		const int NewX = Tile->Coordinates.X, NewY = Tile->Coordinates.Y;
		const int OldX = CurrentTile->Coordinates.X, OldY = CurrentTile->Coordinates.Y;
	
		/* Test if the tile's coordinates match with any of this piece's move patterns.
		 * The Guardian can move in any diagonal direction, up to 2 tiles. */
		if
		(
			/* Forward 1, right 1. */
			(NewX == OldX + 1 && NewY == OldY + 1) ||
			/* Forward 1, left 1. */
			(NewX == OldX - 1 && NewY == OldY + 1) ||
			/* Backward 1, right 1. */
			(NewX == OldX + 1 && NewY == OldY - 1) ||
			/* Backward 1, left 1. */
			(NewX == OldX - 1 && NewY == OldY - 1) ||

			/* Forward 2, right 2. */
			(NewX == OldX + 2 && NewY == OldY + 2) ||
			/* Forward 2, left 2. */
			(NewX == OldX - 2 && NewY == OldY + 2) ||
			/* Backward 2, right 2. */
			(NewX == OldX + 2 && NewY == OldY - 2) ||
			/* Backward 2, left 2. */
			(NewX == OldX - 2 && NewY == OldY - 2)
		)
		{
			return true;
		}
	}

	return false;
}
