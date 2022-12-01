// Copyright Samuel Reitich 2023.


#include "Pieces/Commanders/King.h"

#include "Board/BoardTile.h"

AKing::AKing()
{
}

bool AKing::TileIsInMoveRange(ABoardTile* Tile)
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
			(NewX == OldX - 1 && NewY == OldY) ||
		
			/* Forward 1, right 1 */
			(NewX == OldX + 1 && NewY == OldY + 1) ||
			/* Forward 1, left 1 */
			(NewX == OldX - 1 && NewY == OldY + 1) ||
			/* Backward 1, right 1 */
			(NewX == OldX + 1 && NewY == OldY - 1) ||
			/* Backward 1, left 1 */
			(NewX == OldX - 1 && NewY == OldY - 1)
		)
		{
			return true;
		}
	}
	
	return false;
}
