// Copyright Change Studios, LLC 2023.


#include "Pieces/Soldiers/WarMage.h"

#include "Board/BoardTile.h"
#include "Components/PieceNetworkingComponent.h"
#include "Framework/Match/Match_PlayerPawn.h"

bool AWarMage::TileIsInMoveRange(ABoardTile* Tile)
{
	/* Make sure that a valid tile was passed. */
	if (IsValid(Tile))
	{
		/* Test if the tile's coordinates match with any of this piece's move patterns. */
		if
		(
			/* Forward 1 */
			CurrentTile->CheckTilePosition(Tile, 1, 0) ||
			/* Backward 1 */
			CurrentTile->CheckTilePosition(Tile, -1, 0) ||

			/* Right 1 */
			CurrentTile->CheckTilePosition(Tile, 0, 1) ||
			/* Right 2 */
			CurrentTile->CheckTilePosition(Tile, 0, 2) ||
			/* Right 3 */
			CurrentTile->CheckTilePosition(Tile, 0, 3) ||
			/* Left 1 */
			CurrentTile->CheckTilePosition(Tile, 0, -1) ||
			/* Left 2 */
			CurrentTile->CheckTilePosition(Tile, 0, -2) ||
			/* Left 3 */
			CurrentTile->CheckTilePosition(Tile, 0, -3)
		)
		{
			return true;
		}
	}
	
	return false;
}

void AWarMage::OnModifierAddedOrRemoved(FModifier Modifier, bool bRemoved)
{
	/* This ability is only triggered when a modifier is added. */
	if (!bRemoved)
	{
		/* Get this piece's information from the piece data table. */
		if (IsValid(PieceDataTable))
		{
			static const FString ContextString(TEXT("Piece Data Struct"));
			FPieceDataStruct* PieceData;
			PieceData = PieceDataTable->FindRow<FPieceDataStruct>(PieceID, ContextString, true);

			/* Ensure this piece's data was found. */
			if (PieceData)
			{
				/* Create a new modifier from this piece's passive ability that adds +1 of the original modifier's
				 * effected stat(s). */
				FModifier BonusModifier = FModifier();
				BonusModifier.SourceActor = this;
				BonusModifier.SourceName = PieceData->PieceName;
				BonusModifier.SourceAbilityName = PieceData->PassiveName;
				if (Modifier.StrengthChange > 0)
					BonusModifier.StrengthChange = 1;
				if (Modifier.ArmorChange > 0)
					BonusModifier.ArmorChange = 1;
				BonusModifier.RemainingDuration = Modifier.RemainingDuration - 1;
				BonusModifier.bStrPopUpPlayed = BonusModifier.StrengthChange == 0;
				BonusModifier.bArmPopUpPlayed = BonusModifier.ArmorChange == 0;
				BonusModifier.bCanStack = false;

				/* Add new modifier to this piece. */
				if (AMatch_PlayerPawn* PlayerPawnPtr = Cast<AMatch_PlayerPawn>(GetInstigator()))
				{
					PlayerPawnPtr->GetPieceNetworkingComponent()->Server_AddModifier(this, BonusModifier, true);
				}
			}
		}
	}
}
