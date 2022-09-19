// Copyright Samuel Reitich 2022.


#include "Pieces/Soldiers/Knight.h"

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
	/* Create an ability confirmation widget. */
	UKnight_ActiveAbilityConfirmation* ActiveAbilityConfirmation = CreateWidget<UKnight_ActiveAbilityConfirmation>(GetWorld(), ActiveAbilityConfirmationClass, FName("Active Ability Confirmation Widget"));
	ActiveAbilityConfirmation->UpdateActionConfirmationInfo(this, Cast<ABoardTile>(Targets[0]));
	ActiveAbilityConfirmation->AddToViewport(0);

	/* Highlight the pending tile. */
	Cast<ABoardTile>(Targets[0])->Highlight->SetMaterial(0, Cast<ABoardTile>(Targets[0])->Highlight_Target);
}

void AKnight::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Call the blueprint implementation of "Dash." */
	if (ABoardTile* TargetTile = Cast<ABoardTile>(Targets[0]))
		BP_OnActiveAbility(Cast<ABoardTile>(TargetTile));
}
