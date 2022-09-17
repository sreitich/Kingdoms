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
	
	/* The dash can move to any tile that the piece can move to with a move action. */
	for (ABoardTile* Tile : GetValidTiles())
	{
		ValidTargets.Add(Cast<AActor>(Tile));
	}

	return ValidTargets;
}

void AKnight::StartActiveConfirmation(TArray<AActor*> Targets)
{
	UKnight_ActiveAbilityConfirmation* ActiveAbilityConfirmation = CreateWidget<UKnight_ActiveAbilityConfirmation>(GetWorld(), ActiveAbilityConfirmationClass, FName("Active Ability Confirmation Widget"));

	ActiveAbilityConfirmation->AddToViewport(0);
}

void AKnight::OnActiveAbility(AActor* Target)
{
	/* Call the blueprint implementation of "Dash." */
	BP_OnActiveAbility(Target);
}
