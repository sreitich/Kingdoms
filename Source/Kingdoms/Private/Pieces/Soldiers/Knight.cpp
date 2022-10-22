// Copyright Samuel Reitich 2022.


#include "Pieces/Soldiers/Knight.h"
#include "UserDefinedData/PieceData_UserDefinedData.h"

#include "Board/BoardTile.h"
#include "UserInterface/Match/ActiveAbilityConfirmations/Soldiers/Knight_ActiveAbilityConfirmation.h"

AKnight::AKnight()
{
}

TArray<AActor*> AKnight::GetValidActiveAbilityTargets()
{
	TArray<AActor*> ValidTargets;
	
	/* The dash can move to any unoccupied tile that the piece can move to with a move action. */
	for (ABoardTile* Tile : GetValidTiles())
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
