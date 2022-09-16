// Copyright Samuel Reitich 2022.


#include "Pieces/Soldiers/Knight.h"

#include "Board/BoardTile.h"
#include "Components/ServerCommunicationComponent.h"
#include "Framework/Match/Match_PlayerController.h"

AKnight::AKnight()
{
}

void AKnight::OnActiveAbility(AActor* Target)
{
	/* Call the blueprint implementation of "Dash." */
	BP_OnActiveAbility(Target);
}

TArray<AActor*> AKnight::GetValidActiveAbilityTargets()
{
	TArray<AActor*> ValidTargets;
	
	/* The dash can move to any tile that the piece can move to with a move action. */
	for (ABoardTile* Tile : GetValidTiles())
	{
		ValidTargets.Add(Cast<AActor>(Tile));
	}

	return ValidTargets;
}
