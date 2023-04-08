// Copyright Change Studios, LLC 2023.


#include "Pieces/Mages/Cryomancer.h"

#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "UserInterface/Match/ActiveAbilityConfirmations/Mages/Cryomancer/Match_CryoActiveConfirmation.h"

void ACryomancer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/* Replicate these variables. */
	DOREPLIFETIME(ACryomancer, ActiveAbilityTarget);
}

bool ACryomancer::TileIsInMoveRange(ABoardTile* Tile)
{
	/* Make sure that a valid tile was passed. */
	if (IsValid(Tile))
	{
		/* Test if the tile's coordinates match with any of this piece's move patterns. */
		if
		(
			/* Right 1 */
			CurrentTile->CheckTilePosition(Tile, 0, 1) ||
			/* Left 1 */
			CurrentTile->CheckTilePosition(Tile, 0, -1) ||

			/* Forward 1 */
			CurrentTile->CheckTilePosition(Tile, 1, 0) ||
			/* Backward 1 */
			CurrentTile->CheckTilePosition(Tile, -1, 0) ||
			/* Forward 2 */
			CurrentTile->CheckTilePosition(Tile, 2, 0) ||
			/* Backward 2 */
			CurrentTile->CheckTilePosition(Tile, 2, 0) ||
			/* Forward 3 */
			CurrentTile->CheckTilePosition(Tile, 3, 0) ||
			/* Backward 3 */
			CurrentTile->CheckTilePosition(Tile, -3, 0)
		)
		{
			return true;
		}
	}

	return false;
}

void ACryomancer::OnActiveClicked()
{
	/* Highlight valid tiles. */
	Super::OnActiveClicked();

	/* Create and update the active ability confirmation widget if it hasn't been created yet. */
	if (!ActiveAbilityConfirmationWidget)
	{
		/* Create an instance of this piece's active ability confirmation widget. */
		ActiveAbilityConfirmationWidget = CreateWidget<UMatch_CryoActiveConfirmation>(Cast<APlayerController>(GetInstigator()->GetController()), ActiveAbilityConfirmationClass, FName(GetName() + " Active Ability Confirmation Widget"));

		/* Update the active ability confirmation widget and add it to the viewport if it was created successfully. */
		if (ActiveAbilityConfirmationWidget)
		{
			Cast<UMatch_CryoActiveConfirmation>(ActiveAbilityConfirmationWidget)->Widget_UpdateActiveConfirmation(this, TArray<AActor*>());
			ActiveAbilityConfirmationWidget->AddToViewport(0);
		}
	}
}

void ACryomancer::Piece_UpdateActiveConfirmation(TArray<AActor*> Targets)
{
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

	/* Update the widget's target. */
	if (IsValid(TargetPiece) && ActiveAbilityConfirmationWidget)
	{
		TArray<AActor*> NewTargets = { TargetPiece };
		Cast<UMatch_CryoActiveConfirmation>(ActiveAbilityConfirmationWidget)->Widget_UpdateActiveConfirmation(this, NewTargets);
	}
}

TArray<AActor*> ACryomancer::GetValidActiveAbilityTargets()
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

TArray<ABoardTile*> ACryomancer::GetActiveAbilityRange()
{
	TArray<ABoardTile*> TilesInRange;

	/* Get the board manager's array of every tile on the board. */
	for (ABoardTile* Tile : GetWorld()->GetGameState<AMatch_GameStateBase>()->BoardManager->AllTiles)
	{
		/* If the tile is at a valid location. */
		if
		(
			/* 1 forward */
			CurrentTile->CheckTilePosition(Tile, 1, 0) ||
			/* 1 backward */
			CurrentTile->CheckTilePosition(Tile, -1, 0) ||
			/* 1 right */
			CurrentTile->CheckTilePosition(Tile, 0, 1) ||
			/* 1 left */
			CurrentTile->CheckTilePosition(Tile, 0, -1) ||
				
			/* 1 forward, 1 right */
			CurrentTile->CheckTilePosition(Tile, 1, 1) ||
			/* 1 forward, 1 left */
			CurrentTile->CheckTilePosition(Tile, 1, -1) ||
			/* 1 backward, 1 right */
			CurrentTile->CheckTilePosition(Tile, -1, 1) ||
			/* 1 backward, 1 left */
			CurrentTile->CheckTilePosition(Tile, -1, -1) ||

			/* 2 forward */
			CurrentTile->CheckTilePosition(Tile, 2, 0) ||
			/* 2 backward */
			CurrentTile->CheckTilePosition(Tile, -2, 0) ||
			/* 2 right */
			CurrentTile->CheckTilePosition(Tile, 0, 2) ||
			/* 2 left */
			CurrentTile->CheckTilePosition(Tile, 0, -2)
		)
		{
			TilesInRange.Add(Tile);
		}
	}

	return TilesInRange;
}

void ACryomancer::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Update the player's turn progress, trigger the ability cooldown, and decrement the ability's limited uses. */
	Super::OnActiveAbility(Targets);

	/* Call the blueprint implementation of this piece's active ability. Piece_UpdateActiveConfirmation
	 * ensures that given targets are always pieces. */
	BP_OnActiveAbility(Cast<AParentPiece>(Targets[0]));
}

void ACryomancer::OnAbilityEffectEnded(TArray<AActor*> Targets)
{
	/* Wrap the multicast implementation of this function, since this is called by the server. */
	Multicast_OnAbilityEffectEnded();
}

void ACryomancer::Multicast_OnAbilityEffectEnded_Implementation()
{
	/* Call the blueprint-implementation of this function on each client. */
	BP_OnAbilityEffectEnded();
}