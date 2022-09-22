// Copyright Samuel Reitich 2022.


#include "Pieces/Mages/Pyromancer.h"
#include "UserInterface/Match/Match_AttackConfirmation.h"

#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"

APyromancer::APyromancer()
{
}

TArray<AActor*> APyromancer::GetValidActiveAbilityTargets()
{
	TArray<AActor*> ValidTargets;

	/* Get the board manager's array of every tile on the board. */
	for (ABoardTile* Tile : GetWorld()->GetGameState<AMatch_GameStateBase>()->BoardManager->AllTiles)
	{
		/* Store the given tile and current tile's coordinates in variables for readability. */
		const int NewX = Tile->Coordinates.X, NewY = Tile->Coordinates.Y;
		const int OldX = CurrentTile->Coordinates.X, OldY = CurrentTile->Coordinates.Y;
		
		/* If the tile is occupied by an enemy piece and in a valid location. */
		if
		(IsValid(Tile->GetOccupyingPiece()) && !Tile->GetOccupyingPiece()->GetInstigator()->IsLocallyControlled() &&
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
			(NewY == OldY + 2 && NewX == OldX + 1) ||
			/* 1 forward, 2 left */
			(NewY == OldY + 2 && NewX == OldX - 1) ||
			/* 1 backward, 2 right */
			(NewY == OldY - 2 && NewX == OldX + 1) ||
			/* 1 backward, 2 left */
			(NewY == OldY - 2 && NewX == OldX - 1) ||

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
		))
		{
			ValidTargets.Add(Cast<AActor>(Tile->GetOccupyingPiece()));
		}
	}

	return ValidTargets;
}

void APyromancer::StartActiveConfirmation(TArray<AActor*> Targets)
{
	/* If the confirmation widget hasn't been created yet, create it. */
	if (!ConfirmationWidget)
	{
		/* Create an ability confirmation widget. */
		ConfirmationWidget = CreateWidget<UMatch_AttackConfirmation>(GetWorld(), ActiveAbilityConfirmationClass, FName("Active Ability Confirmation Widget"));
		ConfirmationWidget->AddToViewport(0);
	}

	if (AParentPiece* Target = Cast<AParentPiece>(Targets[0])) // causing crash
	{
		/* Update the widget's information. */
		ConfirmationWidget->UpdateAttackPreviewInfo(this, Target);

		/* Highlight the pending tile. */
		Cast<ABoardTile>(Target->GetCurrentTile())->Highlight->SetMaterial(0, Cast<ABoardTile>(Targets[0])->Highlight_Target);
	}
}

void APyromancer::OnActiveAbility(TArray<AActor*> Targets)
{
	Super::OnActiveAbility(Targets);

}