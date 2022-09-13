// Copyright Samuel Reitich 2022.


#include "Pieces/Soldiers/Knight.h"

#include "Board/BoardTile.h"

AKnight::AKnight()
{
}

void AKnight::OnActiveAbility(AActor* Target)
{
	Super::OnActiveAbility(Target);

	UE_LOG(LogTemp, Error, TEXT("DASHED!"));
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
