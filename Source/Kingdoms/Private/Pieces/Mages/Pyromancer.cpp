// Copyright Change Studios, LLC 2023.


#include "Pieces/Mages/Pyromancer.h"

#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "Framework/Match/Match_PlayerPawn.h"

#include "UserInterface/Match/ActiveAbilityConfirmations/Mages/Pyromancer/Match_PyroActiveConfirmation.h"

#include "Kismet/GameplayStatics.h"

APyromancer::APyromancer()
{
}

bool APyromancer::TileIsInMoveRange(ABoardTile* Tile)
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
			/* Backward 1 */
			(NewX == OldX && NewY == OldY - 1) ||

			/* Right 1 */
			(NewX == OldX + 1 && NewY == OldY) ||
			/* Left 1 */
			(NewX == OldX - 1 && NewY == OldY) ||
			/* Right 2 */
			(NewX == OldX + 2 && NewY == OldY) ||
			/* Left 2 */
			(NewX == OldX - 2 && NewY == OldY) ||
			/* Right 3 */
			(NewX == OldX + 3 && NewY == OldY) ||
			/* Left 3 */
			(NewX == OldX - 3 && NewY == OldY)
		)
		{
			return true;
		}
	}

	return false;
}

void APyromancer::OnActiveClicked()
{
	/* Highlight valid targets. */
	Super::OnActiveClicked();

	/* Create and update the active ability confirmation widget if it hasn't been created yet. */
	if (!ActiveAbilityConfirmationWidget)
	{
		/* Create an instance of this piece's active ability confirmation widget. */
		ActiveAbilityConfirmationWidget = CreateWidget<UMatch_PyroActiveConfirmation>(GetWorld(), ActiveAbilityConfirmationClass, FName("Pyromancer Active Ability Confirmation Widget"));

		/* Update the active ability confirmation widget and add it to the viewport if it was created successfully. */
		if (ActiveAbilityConfirmationWidget)
		{
			/* Display the preview with what the pyromancer's stats would be if the player used its ability. */
			const FAttackPreviewInfo AttackPreviewInfo =
			{
				this,
				__nullptr,
				GetCurrentStrength() + StrengthBuffValue,
				GetCurrentArmor(),
				0,
				0,
				true
			};

			/* This piece's active ability confirmation widget is actually an attack confirmation widget. Update the
			 * widget's friendly piece information, hide its enemy piece information, and disable the confirmation
			 * option. */
			Cast<UMatch_PyroActiveConfirmation>(ActiveAbilityConfirmationWidget)->UpdateAttackPreviewInfo(AttackPreviewInfo);
			ActiveAbilityConfirmationWidget->AddToViewport(0);
		}
	}
}

void APyromancer::Piece_UpdateActiveConfirmation(TArray<AActor*> Targets)
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

	if (IsValid(TargetPiece) && ActiveAbilityConfirmationWidget)
	{
		/* Display the preview with what the pyromancer's stats would be if the player used its ability. */
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

		Cast<UMatch_PyroActiveConfirmation>(ActiveAbilityConfirmationWidget)->UpdateAttackPreviewInfo(AttackPreviewInfo);
	}
}

TArray<AActor*> APyromancer::GetValidActiveAbilityTargets()
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

TArray<ABoardTile*> APyromancer::GetActiveAbilityRange()
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

			/* 1 forward, 2 right */
			(NewY == OldY + 1 && NewX == OldX + 2) ||
			/* 1 forward, 2 left */
			(NewY == OldY + 1 && NewX == OldX - 2) ||
			/* 1 backward, 2 right */
			(NewY == OldY - 1 && NewX == OldX + 2) ||
			/* 1 backward, 2 left */
			(NewY == OldY - 1 && NewX == OldX - 2) ||

			/* 2 forward */
			(NewY == OldY + 2 && NewX == OldX) ||
			/* 2 backward */
			(NewY == OldY - 2 && NewX == OldX) ||
			/* 2 right */
			(NewY == OldY && NewX == OldX + 2) ||
			/* 2 left */
			(NewY == OldY && NewX == OldX - 2) ||

			/* 2 forward, 1 right */
			(NewY == OldY + 2 && NewX == OldX + 1) ||
			/* 2 forward, 1 left */
			(NewY == OldY + 2 && NewX == OldX - 1) ||
			/* 2 backward, 1 right */
			(NewY == OldY - 2 && NewX == OldX + 1) ||
			/* 2 backward, 1 left */
			(NewY == OldY - 2 && NewX == OldX - 1) ||

			/* 3 forward */
			(NewY == OldY + 3 && NewX == OldX) ||
			/* 3 backward */
			(NewY == OldY - 3 && NewX == OldX) ||
			/* 3 right */
			(NewY == OldY && NewX == OldX + 3) ||
			/* 3 left */
			(NewY == OldY && NewX == OldX - 3)
		)
		{
			TilesInRange.Add(Tile);
		}
	}

	return TilesInRange;
}

void APyromancer::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Update the player's turn progress, trigger the ability cooldown, and decrement the ability's limited uses. */
	Super::OnActiveAbility(Targets);

	/* Call the blueprint implementation of the ability, which begins the power-up phase. Piece_UpdateActiveConfirmation
	 * ensures that given targets are always pieces. */
	BP_OnActiveAbility(Cast<AParentPiece>(Targets[0]));
}

void APyromancer::OnAbilityEffectEnded(TArray<AActor*> Targets)
{
	/* TO-DO: Toggle off the Pyromancer's flaming hands. */
	// Cast<UAnimInstance_Parent>(GetMesh()->GetAnimInstance());
}
