// Copyright Samuel Reitich 2022.


#include "Pieces/Mages/Pyromancer.h"

#include "Animations/AnimInstance_Parent.h"
#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "UserInterface/Match/ActiveAbilityConfirmations/Mages/Pyromancer/Match_PyroActiveConfirmation.h"

#include "Kismet/GameplayStatics.h"

APyromancer::APyromancer()
{
}

TArray<ABoardTile*> APyromancer::GetValidMoveTiles()
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

TArray<AActor*> APyromancer::GetValidActiveAbilityTargets()
{
	TArray<AActor*> ValidTargets;

	for (AActor* Actor : GetActiveAbilityRange())
	{
		if (ABoardTile* Tile = Cast<ABoardTile>(Actor))
		{
			/* Tiles can only be targeted if they're occupied by an enemy piece and the path to them is clear. */
			if (IsValid(Tile->GetOccupyingPiece()) && !Tile->GetOccupyingPiece()->GetInstigator()->IsLocallyControlled() && PathToTileIsClear(Tile))
			{
				/* The player can select the piece or its tile. */
				ValidTargets.Add(Cast<AActor>(Tile->GetOccupyingPiece()));
				ValidTargets.Add(Cast<AActor>(Tile));
			}
		}
	}

	return ValidTargets;
}

TArray<AActor*> APyromancer::GetActiveAbilityRange()
{
	TArray<AActor*> TargetsInRange;

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
			TargetsInRange.Add(Cast<AActor>(Tile));
		}
	}

	return TargetsInRange;
}

void APyromancer::StartActiveConfirmation(TArray<AActor*> Targets)
{
	/* If the confirmation widget hasn't been created yet, create it. */
	if (!ConfirmationWidget)
	{
		/* Create an ability confirmation widget. */
		ConfirmationWidget = CreateWidget<UMatch_PyroActiveConfirmation>(GetWorld(), ActiveAbilityConfirmationClass, FName("Fireball Ability Confirmation Widget"));
		ConfirmationWidget->ConfirmingPiece = this;
		ConfirmationWidget->AddToViewport(0);
	}

	/* Get the target as a piece. If the player clicked a piece, cast it to a piece pointer. If they clicked
	 * a tile, get its occupying piece. */
	AParentPiece* Target = nullptr;
	if (AParentPiece* TargetPiece = Cast<AParentPiece>(Targets[0]))
	{
		Target = TargetPiece;
	}
	else if (const ABoardTile* TargetTile = Cast<ABoardTile>(Targets[0]))
	{
		Target = TargetTile->GetOccupyingPiece();
	}
	else
	{
		/* Only pieces and tiles can be targeted, so this should never happen. */
	}

	/* If the target piece was successfully found, update the confirmation widget's info and highlight
	 * the pending tile. */
	if (IsValid(Target))
	{
		/* Update the widget's information. */
		ConfirmationWidget->UpdateAttackPreviewInfo(this, Target);

		/* Highlight the pending tile. */
		// Cast<ABoardTile>(Target->GetCurrentTile())->Highlight->SetMaterial(0, Target->GetCurrentTile()->Highlight_Target);
	}
}

void APyromancer::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Call the blueprint implementation of the ability, which begins the power-up phase. */
	BP_OnActiveAbility(Cast<AParentPiece>(Targets[0]));

	/* If the piece data table was found... */
	if (PieceDataTable)
	{
		/* Get this piece's row from the piece data. */
		static const FString ContextString(TEXT("Piece Data Struct"));
		const FPieceDataStruct* PieceData = PieceDataTable->FindRow<FPieceDataStruct>(PieceID, ContextString, true);

		/* If the data table row was found... */
		if (PieceData)
		{
			/* Put the ability onto cooldown. */
			SetActiveCD(PieceData->ActiveCD);
		}
	}
}

void APyromancer::OnActiveEffectEnded(TArray<AActor*> Targets)
{
	/* TO-DO: Toggle off the Pyromancer's flaming hands. */
	// Cast<UAnimInstance_Parent>(GetMesh()->GetAnimInstance());
}
