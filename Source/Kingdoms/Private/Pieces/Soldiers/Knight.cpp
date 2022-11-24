// Copyright Samuel Reitich 2022.


#include "Pieces/Soldiers/Knight.h"

#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"
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

void AKnight::StartActiveConfirmation(TArray<AActor*> Targets)
{
	/* If the confirmation widget hasn't been created yet, create it. */
	if (!ConfirmationWidget)
	{
		/* Create an ability confirmation widget. */
		ConfirmationWidget = CreateWidget<UKnight_ActiveAbilityConfirmation>(GetWorld(), ActiveAbilityConfirmationClass, FName("Active Ability Confirmation Widget"));
		ConfirmationWidget->AddToViewport(0);
	}

	/* Update the widget's information. */
	ConfirmationWidget->UpdateActionConfirmationInfo(this, Cast<ABoardTile>(Targets[0]));
}

void AKnight::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Updates the player's turn progress. */
	Super::OnActiveAbility(Targets);

	/* Call the blueprint implementation of "Dash." */
	if (ABoardTile* TargetTile = Cast<ABoardTile>(Targets[0]))
		BP_OnActiveAbility(Cast<ABoardTile>(TargetTile));

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
