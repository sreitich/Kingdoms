// Copyright Change Studios, LLC 2023.

#include "Pieces/Paladins/Captain.h"

#include "Animations/AnimInstance_Parent.h"
#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Components/PieceNetworkingComponent.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"

bool ACaptain::TileIsInMoveRange(ABoardTile* Tile)
{
	/* Make sure that a valid tile was passed. */
	if (IsValid(Tile))
	{
		/* Test if the tile's coordinates match with any of this piece's move patterns.
		 * The Guardian can move in any diagonal direction, up to 2 tiles. */
		if
		(
			/* Forward 1, right 1. */
			CurrentTile->CheckTilePosition(Tile, 1, 1) ||
			/* Forward 1, left 1. */
			CurrentTile->CheckTilePosition(Tile, 1, -1) ||
			/* Backward 1, right 1. */
			CurrentTile->CheckTilePosition(Tile, -1, 1) ||
			/* Backward 1, left 1. */
			CurrentTile->CheckTilePosition(Tile, -1, -1) ||

			/* Forward 2, right 2. */
			CurrentTile->CheckTilePosition(Tile, 2, 2) ||
			/* Forward 2, left 2. */
			CurrentTile->CheckTilePosition(Tile, 2, -2) ||
			/* Backward 2, right 2. */
			CurrentTile->CheckTilePosition(Tile, -2, 2) ||
			/* Backward 2, left 2. */
			CurrentTile->CheckTilePosition(Tile, -2, -2)
		)
		{
			return true;
		}
	}

	return false;
}

TArray<AActor*> ACaptain::GetValidPassiveAbilityTargets()
{
	/* Array to return. */
	TArray<AActor*> ValidTargets;

	/* Iterate through every tile within range. Each tile occupied by a friendly piece is a valid target, as well as
	 * its occupying piece. */
	for (ABoardTile* Tile : GetPassiveAbilityRange())
	{
		if (IsValid(Tile->GetOccupyingPiece()) && Tile->GetOccupyingPiece()->GetLocalAlignment() == GetLocalAlignment())
		{
			ValidTargets.Add(Tile);
			ValidTargets.Add(Tile->GetOccupyingPiece());
		}
	}

	return ValidTargets;
}

TArray<ABoardTile*> ACaptain::GetPassiveAbilityRange()
{
	/* Array to return. */
	TArray<ABoardTile*> TilesInRange;

	/* Get the board manager's array of every tile on the board. */
	for (ABoardTile* Tile : GetWorld()->GetGameState<AMatch_GameStateBase>()->BoardManager->AllTiles)
	{
		/* All tiles adjacent to this piece are within this piece's range. */
		if (CurrentTile->IsAdjacentTo(false, Tile))
		{
			TilesInRange.Add(Tile);
		}
	}

	return TilesInRange;
}

void ACaptain::OnGameStart()
{
	/* Call OnTurnStart at the start of the game if this piece is owned by Player 1, who will always take their turn
	 * first. */
	if (AMatch_PlayerState* PlayerStatePtr = Cast<AMatch_PlayerState>(GetInstigator()->GetPlayerState()))
	{
		if (PlayerStatePtr->PlayerIndex == 1)
		{
			OnTurnStart();
		}
	}
}

void ACaptain::OnTurnStart()
{
	/* If there are valid passive ability targets at the start of each turn, activate this piece's passive ability
	 * animation, which activates its passive ability via an animation notify. */
	if (GetValidPassiveAbilityTargets().Num() > 0)
	{
		Multicast_AnimatePassiveAbility();
	}
}

void ACaptain::Multicast_AnimatePassiveAbility_Implementation()
{
	/* Activate this piece's passive ability animation, activating its passive ability via a notify. */
	if (UAnimInstance_Parent* AnimInstancePtr = Cast<UAnimInstance_Parent>(GetMesh()->GetAnimInstance()))
	{
		AnimInstancePtr->bUsingPassive = true;
	}
}

void ACaptain::TriggerOnPassiveAbility()
{
	/* This function is a blueprint-exposed wrapper for OnPassiveAbility. */
	OnPassiveAbility(TArray<AActor*>());
}

void ACaptain::OnPassiveAbility(TArray<AActor*> Targets)
{
	/* Get this piece's row from the piece data. */
	static const FString ContextString(TEXT("Piece Data Struct"));
	const FPieceDataStruct* PieceData = PieceDataTable->FindRow<FPieceDataStruct>(PieceID, ContextString, true);

	if (PieceData)
	{
		/* Create a modifier to add to adjacent friendly pieces. */
		FModifier ModifierToAdd = FModifier();
		ModifierToAdd.SourceActor = this;
		ModifierToAdd.SourceAlignmentToTarget = E_Friendly;
		ModifierToAdd.SourceName = PieceData->PieceName;
		ModifierToAdd.SourceAbilityName = PieceData->PassiveName;
		ModifierToAdd.StrengthChange = PassiveAbilityStrengthChange;
		ModifierToAdd.ArmorChange = PassiveAbilityArmorChange;
		ModifierToAdd.RemainingDuration = 1;
		ModifierToAdd.bStrPopUpPlayed = !PassiveAbilityStrengthChange;
		ModifierToAdd.bArmPopUpPlayed = !PassiveAbilityArmorChange;
		ModifierToAdd.bCanStack = false;

		/* Add the modifier to every adjacent friendly piece. */
		for (AActor* PieceActor : GetValidPassiveAbilityTargets())
		{
			if (AParentPiece* Piece = Cast<AParentPiece>(PieceActor))
			{
				if (PieceData)
				{
					Cast<AMatch_PlayerPawn>(GetInstigator())->GetPieceNetworkingComponent()->Server_AddModifier(Piece, ModifierToAdd, true);
				}
			}
		}
	}
}