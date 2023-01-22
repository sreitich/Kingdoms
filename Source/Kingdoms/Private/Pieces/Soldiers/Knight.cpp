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
