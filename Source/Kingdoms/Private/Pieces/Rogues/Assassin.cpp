// Copyright Change Studios, LLC 2023.


#include "Pieces/Rogues/Assassin.h"

#include "Board/BoardTile.h"

bool AAssassin::TileIsInMoveRange(ABoardTile* Tile)
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
			/* Forward 1, right 1 */
			(NewX == OldX + 1 && NewY == OldY + 1) ||
			/* Forward 1, left 1 */
			(NewX == OldX - 1 && NewY == OldY + 1) ||
			/* Backward 1, right 1 */
			(NewX == OldX + 1 && NewY == OldY - 1) ||
			/* Backward 1, left 1 */
			(NewX == OldX - 1 && NewY == OldY - 1) ||
	
			/* Forward 3, right 1 */
			(NewX == OldX + 1 && NewY == OldY + 3) ||
			/* Forward 3, left 1 */
			(NewX == OldX - 1 && NewY == OldY + 3) ||
			/* Backward 3, right 1 */
			(NewX == OldX + 1 && NewY == OldY - 3) ||
			/* Backward 3, left 1 */
			(NewX == OldX - 1 && NewY == OldY - 3) ||

			/* Forward 1, right 3 */
			(NewX == OldX + 3 && NewY == OldY + 1) ||
			/* Forward 1, left 3 */
			(NewX == OldX - 3 && NewY == OldY + 1) ||
			/* Backward 1, right 3 */
			(NewX == OldX + 3 && NewY == OldY - 1) ||
			/* Backward 1, left 3 */
			(NewX == OldX - 3 && NewY == OldY - 1)
		)
		{
			return true;
		}
	}
	
	return false;
}
