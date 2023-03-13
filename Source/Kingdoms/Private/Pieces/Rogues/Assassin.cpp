// Copyright Change Studios, LLC 2023.


#include "Pieces/Rogues/Assassin.h"

#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "UserInterface/Match/ActiveAbilityConfirmations/Rogues/Match_AssassinActiveConfirmation.h"

TArray<ABoardTile*> AAssassin::GetValidMoveTiles()
{
	/* This array of valid tiles is going to be returned. */
	TArray<ABoardTile*> ValidTiles;

	/* Get the board manager's array of every tile on the board. */
	for (ABoardTile* Tile : GetWorld()->GetGameState<AMatch_GameStateBase>()->BoardManager->AllTiles)
	{
		/* If the tile's coordinates match with one of this piece's move patterns and it is empty or not occupied by a
		 * friendly piece, it is a valid destination. This piece can move through other pieces. */
		if (TileIsInMoveRange(Tile) && (!IsValid(Tile->GetOccupyingPiece()) || Tile->GetOccupyingPiece()->GetLocalAlignment() != GetLocalAlignment()))
		{
			ValidTiles.Add(Tile);
		}
	}

	return ValidTiles;
}

bool AAssassin::TileIsInMoveRange(ABoardTile* Tile)
{
	/* Make sure that a valid tile was passed. */
	if (IsValid(Tile))
	{
		/* Test if the tile's coordinates match with any of this piece's move patterns. */
		if
		(
			/* Forward 1, right 1 */
			CurrentTile->CheckTilePosition(Tile, 1, 1) ||
			/* Forward 1, left 1 */
			CurrentTile->CheckTilePosition(Tile, 1, -1) ||
			/* Backward 1, right 1 */
			CurrentTile->CheckTilePosition(Tile, -1, 1) ||
			/* Backward 1, left 1 */
			CurrentTile->CheckTilePosition(Tile, -1, -1) ||
	
			/* Forward 3, right 1 */
			CurrentTile->CheckTilePosition(Tile, 3, 1) ||
			/* Forward 3, left 1 */
			CurrentTile->CheckTilePosition(Tile, 3, -1) ||
			/* Backward 3, right 1 */
			CurrentTile->CheckTilePosition(Tile, -3, 1) ||
			/* Backward 3, left 1 */
			CurrentTile->CheckTilePosition(Tile, -3, -1) ||

			/* Forward 1, right 3 */
			CurrentTile->CheckTilePosition(Tile, 1, 3) ||
			/* Forward 1, left 3 */
			CurrentTile->CheckTilePosition(Tile, 1, -3) ||
			/* Backward 1, right 3 */
			CurrentTile->CheckTilePosition(Tile, -1, 3) ||
			/* Backward 1, left 3 */
			CurrentTile->CheckTilePosition(Tile, -1, -3)
		)
		{
			return true;
		}
	}
	
	return false;
}

void AAssassin::OnActiveClicked()
{
	/* Highlight valid targets. */
	Super::OnActiveClicked();

	/* Create and update the active ability confirmation widget if it hasn't been created yet. */
	if (!ActiveAbilityConfirmationWidget)
	{
		/* Create an instance of this piece's active ability confirmation widget. */
		ActiveAbilityConfirmationWidget = CreateWidget<UMatch_AssassinActiveConfirmation>(Cast<APlayerController>(GetInstigator()->GetController()), ActiveAbilityConfirmationClass, FName("Assassin Active Ability Confirmation Widget"));

		/* Update the active ability confirmation widget and add it to the viewport if it was created successfully. */
		if (ActiveAbilityConfirmationWidget)
		{
			/* Display the preview with what the Assassin's stats would be if the player used its ability. */
			const FAttackPreviewInfo AttackPreviewInfo =
			{
				this,
				nullptr,
				GetCurrentStrength() + StrengthBuffValue,
				GetCurrentArmor(),
				0,
				0,
				true
			};

			/* This piece's active ability confirmation widget is actually an attack confirmation widget. Update the
			 * widget's friendly piece information, hide its enemy piece information, and disable the confirmation
			 * option. */
			Cast<UMatch_AssassinActiveConfirmation>(ActiveAbilityConfirmationWidget)->UpdateAttackPreviewInfo(AttackPreviewInfo);
			ActiveAbilityConfirmationWidget->AddToViewport(0);
		}
	}
}

void AAssassin::Piece_UpdateActiveConfirmation(TArray<AActor*> Targets)
{
	Super::Piece_UpdateActiveConfirmation(Targets);

	/* The player can select either a piece or its current tile. Either way, get the target as a piece. */
	AParentPiece* TargetPiece = nullptr;
	if (AParentPiece* TargetToPiece = Cast<AParentPiece>(Targets[0]))
	{
		/* Get the target piece from the selected piece actor. */
		TargetPiece = TargetToPiece;
	}
	else if (ABoardTile* TargetToTile = Cast<ABoardTile>(Targets[0]))
	{
		/* Get the target piece from the selected tile. */
		TargetPiece = TargetToTile->GetOccupyingPiece();
	}

	if (IsValid(TargetPiece) && ActiveAbilityConfirmationWidget)
	{
		/* Display the preview with what the Assassin's stats would be if the player used its ability. */
		const FAttackPreviewInfo AttackPreviewInfo =
		{
			this,
			TargetPiece,
			GetCurrentStrength() + StrengthBuffValue,
			GetCurrentArmor(),
			TargetPiece->GetCurrentStrength(),
			TargetPiece->GetCurrentArmor(),
			true
		};

		Cast<UMatch_AssassinActiveConfirmation>(ActiveAbilityConfirmationWidget)->UpdateAttackPreviewInfo(AttackPreviewInfo);
	}
}

TArray<AActor*> AAssassin::GetValidActiveAbilityTargets()
{
	TArray<AActor*> ValidTargets;

	/* Returns all valid target pieces and their current tiles, so players can select either one. */
	for (ABoardTile* Tile : GetActiveAbilityRange())
	{
		/* Tiles can only be targeted if they're occupied by an enemy piece and the path to them is clear. */
		if (IsValid(Tile->GetOccupyingPiece()) && Tile->GetOccupyingPiece()->GetLocalAlignment() != GetLocalAlignment() && PathToTileIsClear(Tile))
		{
			/* The player can select the piece or its tile. */
			ValidTargets.Add(Cast<AActor>(Tile->GetOccupyingPiece()));
			ValidTargets.Add(Cast<AActor>(Tile));
		}
	}

	return ValidTargets;
}

TArray<ABoardTile*> AAssassin::GetActiveAbilityRange()
{
	TArray<ABoardTile*> TilesInRange;

	/* Get the board manager's array of every tile on the board. */
	for (ABoardTile* Tile : GetWorld()->GetGameState<AMatch_GameStateBase>()->BoardManager->AllTiles)
	{
		/* If the tile is at a valid location. */
		if
		(
			/* 1 backward */
			CurrentTile->CheckTilePosition(Tile, -1, 0)
		)
		{
			TilesInRange.Add(Tile);
		}
	}

	return TilesInRange;
}

void AAssassin::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Update the player's turn progress, trigger the ability cooldown, and decrement the ability's limited uses. */
	Super::OnActiveAbility(Targets);

	/* Call the blueprint implementation of the ability, which gives the Assassin its strength modifiers and
	 * starts the attack. Piece_UpdateActiveConfirmation ensures that given targets are always pieces. */
	BP_OnActiveAbility(Cast<AParentPiece>(Targets[0]));
}
