// Copyright Change Studios, LLC 2023.


#include "Pieces/Paladins/Guardian.h"

#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "UserInterface/Match/ActiveAbilityConfirmations/Paladins/Guardian_ActiveConfirmation.h"

void AGuardian::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/* Replicate these variables. */
	DOREPLIFETIME(AGuardian, ActiveAbilityTarget);
}

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

void AGuardian::OnActiveClicked()
{
	/* Highlight valid tiles. */
	Super::OnActiveClicked();

	/* Create and update the active ability confirmation widget if it hasn't been created yet. */
	if (!ActiveAbilityConfirmationWidget)
	{
		/* Create an instance of this piece's active ability confirmation widget. */
		ActiveAbilityConfirmationWidget = CreateWidget<UGuardian_ActiveConfirmation>(Cast<APlayerController>(GetInstigator()->GetController()), ActiveAbilityConfirmationClass, FName("Guardian Active Ability Confirmation Widget"));

		/* Update the active ability confirmation widget and add it to the viewport if it was created successfully. */
		if (ActiveAbilityConfirmationWidget)
		{
			Cast<UGuardian_ActiveConfirmation>(ActiveAbilityConfirmationWidget)->Widget_UpdateActiveConfirmation(this, TArray<AActor*>());
			ActiveAbilityConfirmationWidget->AddToViewport(0);
		}
	}
}

void AGuardian::Piece_UpdateActiveConfirmation(TArray<AActor*> Targets)
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
		Cast<UGuardian_ActiveConfirmation>(ActiveAbilityConfirmationWidget)->Widget_UpdateActiveConfirmation(this, NewTargets);
	}
}

TArray<AActor*> AGuardian::GetValidActiveAbilityTargets()
{
	TArray<AActor*> ValidTargets;

	/* Returns all valid target pieces and their current tiles, so players can select either one. */
	for (ABoardTile* Tile : GetActiveAbilityRange())
	{
		/* Tiles can only be targeted if they're occupied by a friendly piece and the path to them is clear. */
		if (IsValid(Tile->GetOccupyingPiece()) && Tile->GetOccupyingPiece()->GetLocalAlignment() == GetLocalAlignment() && PathToTileIsClear(Tile))
		{
			/* The player can select the piece or its tile. */
			ValidTargets.Add(Cast<AActor>(Tile->GetOccupyingPiece()));
			ValidTargets.Add(Cast<AActor>(Tile));
		}
	}

	return ValidTargets;
}

TArray<ABoardTile*> AGuardian::GetActiveAbilityRange()
{
	TArray<ABoardTile*> TilesInRange;

	/* Get the board manager's array of every tile on the board. */
	for (ABoardTile* Tile : GetWorld()->GetGameState<AMatch_GameStateBase>()->BoardManager->AllTiles)
	{
		/* Store the given tile and current tile's coordinates in variables for readability. */
		const int NewX = Tile->Coordinates.X, NewY = Tile->Coordinates.Y;
		const int OldX = CurrentTile->Coordinates.X, OldY = CurrentTile->Coordinates.Y;
		
		/* If the tile is at a valid location. */
		if
		(
			/* 1 forward */
			(NewY == OldY + 1 && NewX == OldX) ||
			/* 1 backward */
			(NewY == OldY - 1 && NewX == OldX) ||
			/* 1 right */
			(NewY == OldY && NewX == OldX + 1) ||
			/* 1 left */
			(NewY == OldY && NewX == OldX - 1) ||
				
			/* 1 forward, 1 right */
			(NewY == OldY + 1 && NewX == OldX + 1) ||
			/* 1 forward, 1 left */
			(NewY == OldY + 1 && NewX == OldX - 1) ||
			/* 1 backward, 1 right */
			(NewY == OldY - 1 && NewX == OldX + 1) ||
			/* 1 backward, 1 left */
			(NewY == OldY - 1 && NewX == OldX - 1) ||

			/* 2 forward */
			(NewY == OldY + 2 && NewX == OldX) ||
			/* 2 backward */
			(NewY == OldY - 2 && NewX == OldX) ||
			/* 2 right */
			(NewY == OldY && NewX == OldX + 2) ||
			/* 2 left */
			(NewY == OldY && NewX == OldX - 2)
		)
		{
			TilesInRange.Add(Tile);
		}
	}

	return TilesInRange;
}

void AGuardian::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Update the player's turn progress, trigger the ability cooldown, and decrement the ability's limited uses. */
	Super::OnActiveAbility(Targets);

	/* Call the blueprint implementation of this piece's active ability. Piece_UpdateActiveConfirmation
	 * ensures that given targets are always pieces. */
	BP_OnActiveAbility(Cast<AParentPiece>(Targets[0]));
}

void AGuardian::OnAbilityEffectEnded(TArray<AActor*> Targets)
{
	/* Wrap the multicast implementation of this function, since this is called by the server. */
	Multicast_OnAbilityEffectEnded();
}

void AGuardian::Multicast_OnAbilityEffectEnded_Implementation()
{
	/* Call the blueprint-implementation of this function on each client. */
	BP_OnAbilityEffectEnded();
}
