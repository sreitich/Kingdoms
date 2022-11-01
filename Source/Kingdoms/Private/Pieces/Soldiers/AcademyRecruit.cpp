// Copyright Samuel Reitich 2023.


#include "Pieces/Soldiers/AcademyRecruit.h"

#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"


AAcademyRecruit::AAcademyRecruit()
{
}

TArray<ABoardTile*> AAcademyRecruit::GetValidMoveTiles()
{
	/* This array of valid tiles is going to be returned. */
	TArray<ABoardTile*> ValidTiles;

	/* Get the board manager's array of every tile on the board. */
	for (ABoardTile* Tile : GetWorld()->GetGameState<AMatch_GameStateBase>()->BoardManager->AllTiles)
	{
		/* If the tile's coordinates match with one of this piece's move patterns and the path to the tile is clear, it is a valid destination. */
		if (TileIsInMoveRange(Tile) && PathToTileIsClear(Tile))
		{
			ValidTiles.Add(Tile);
		}
	}

	return ValidTiles;
}

bool AAcademyRecruit::TileIsInMoveRange(ABoardTile* Tile)
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
		
			/* Forward 2 */
			(NewX == OldX && NewY == OldY + 2) ||
			/* Right 2 */
			(NewX == OldX + 2 && NewY == OldY) ||
			/* Backward 2 */
			(NewX == OldX && NewY == OldY - 2) ||
			/* Left 2 */
			(NewX == OldX - 2 && NewY == OldY)
		)
		{
			return true;
		}
	}
	
	return false;
}