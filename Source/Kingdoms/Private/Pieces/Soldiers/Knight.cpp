// Copyright Change Studios, LLC 2023.


#include "Pieces/Soldiers/Knight.h"

#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "UserInterface/Match/ActiveAbilityConfirmations/Soldiers/Knight_ActiveAbilityConfirmation.h"

AKnight::AKnight()
{
}

bool AKnight::TileIsInMoveRange(ABoardTile* Tile)
{
	/* Make sure that a valid tile was passed. */
	if (IsValid(Tile))
	{
		/* Test if the tile's coordinates match with any of this piece's move patterns. */
		if
		(
			/* Forward 1 */
			CurrentTile->CheckTilePosition(Tile, 1, 0) ||
			/* Right 1 */
			CurrentTile->CheckTilePosition(Tile, 0, 1) ||
			/* Backward 1 */
			CurrentTile->CheckTilePosition(Tile, -1, 0) ||
			/* Left 1 */
			CurrentTile->CheckTilePosition(Tile, 0, -1) ||
		
			/* Forward 2 */
			CurrentTile->CheckTilePosition(Tile, 2, 0) ||
			/* Right 2 */
			CurrentTile->CheckTilePosition(Tile, 0, 2) ||
			/* Backward 2 */
			CurrentTile->CheckTilePosition(Tile, -2, 0) ||
			/* Left 2 */
			CurrentTile->CheckTilePosition(Tile, 0, -2)
		)
		{
			return true;
		}
	}
	
	return false;
}

void AKnight::OnActiveClicked()
{
	/* Highlight valid tiles. */
	Super::OnActiveClicked();

	/* Create and update the active ability confirmation widget if it hasn't been created yet. */
	if (!ActiveAbilityConfirmationWidget)
	{
		/* Create an instance of this piece's active ability confirmation widget. */
		ActiveAbilityConfirmationWidget = CreateWidget<UKnight_ActiveAbilityConfirmation>(Cast<APlayerController>(GetInstigator()->GetController()), ActiveAbilityConfirmationClass, FName("Knight Active Ability Confirmation Widget"));

		/* Update the active ability confirmation widget and add it to the viewport if it was created successfully. */
		if (ActiveAbilityConfirmationWidget)
		{
			Cast<UKnight_ActiveAbilityConfirmation>(ActiveAbilityConfirmationWidget)->Widget_UpdateActiveConfirmation(this, TArray<AActor*>());
			ActiveAbilityConfirmationWidget->AddToViewport(0);
		}
	}
}

void AKnight::Piece_UpdateActiveConfirmation(TArray<AActor*> Targets)
{
	/* Update the active ability confirmation widget if it has been created. */
	if (ActiveAbilityConfirmationWidget)
	{
		Cast<UKnight_ActiveAbilityConfirmation>(ActiveAbilityConfirmationWidget)->Widget_UpdateActiveConfirmation(this, Targets);
	}
}

TArray<AActor*> AKnight::GetValidActiveAbilityTargets()
{
	TArray<AActor*> ValidTargets;
	
	/* The dash can move to any unoccupied tile that the piece can move to with a move action. */
	for (ABoardTile* Tile : GetValidMoveTiles())
	{
		/* Add the tile to the valid targets array if the tile is unoccupied. */
		if (!IsValid(Tile->GetOccupyingPiece()))
			ValidTargets.Add(Cast<AActor>(Tile));
	}

	return ValidTargets;
}

TArray<ABoardTile*> AKnight::GetActiveAbilityRange()
{
	/* The array of tiles in range that will be returned. */
	TArray<ABoardTile*> TilesInRange;

	/* Get the board manager's array of every tile on the board. */
	for (ABoardTile* Tile : GetWorld()->GetGameState<AMatch_GameStateBase>()->BoardManager->AllTiles)
	{
		/* If this tile is within range, add it to the array of tiles in range. */
		if (TileIsInMoveRange(Tile))
		{
			TilesInRange.Add(Tile);
		}
	}

	/* Return the array of every tile in range. */
	return TilesInRange;
}

void AKnight::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Updates the player's turn progress, trigger the ability cooldown, and decrement the ability's limited uses. */
	Super::OnActiveAbility(Targets);

	/* Call the blueprint implementation of "Dash." */
	if (ABoardTile* TargetTile = Cast<ABoardTile>(Targets[0]))
		BP_OnActiveAbility(Cast<ABoardTile>(TargetTile));
}
