// Copyright Samuel Reitich 2022.


#include "Pieces/Mages/Pyromancer.h"

#include "Animations/AnimInstance_Parent.h"
#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Framework/Match/Match_PlayerState.h"
#include "UserInterface/Match/ActiveAbilityConfirmations/Mages/Pyromancer/Match_PyroActiveConfirmation.h"

#include "Kismet/GameplayStatics.h"

APyromancer::APyromancer()
{
}

void APyromancer::OnActiveClicked()
{
	for (AActor* TileActor : GetActiveAbilityRange())
	{
		if (const ABoardTile* Tile = Cast<ABoardTile>(TileActor))
		{
			/* If the tile is empty, it's in a valid range but isn't a valid target. */
			if (!IsValid(Tile->GetOccupyingPiece()))
			{
				Tile->Highlight->SetMaterial(0, Tile->Highlight_ValidRange);
			}
			/* If the tile is occupied by a friendly piece, highlight it within range but invalid. */
			else if (IsValid(Tile->GetOccupyingPiece()) && Tile->GetOccupyingPiece()->GetInstigator()->IsLocallyControlled())
			{
				Tile->Highlight->SetMaterial(0, Tile->Highlight_ValidFriendlyRange);
			}
			/* If the tile is occupied by an enemy piece, it's a valid target. */
			else if (IsValid(Tile->GetOccupyingPiece()) && !Tile->GetOccupyingPiece()->GetInstigator()->IsLocallyControlled())
			{
				Tile->Highlight->SetMaterial(0, Tile->Highlight_ValidEnemy);
			}
		}
	}
}

TArray<AActor*> APyromancer::GetValidActiveAbilityTargets()
{
	TArray<AActor*> ValidTargets;

	for (AActor* Actor : GetActiveAbilityRange())
	{
		if (ABoardTile* Tile = Cast<ABoardTile>(Actor))
		{
			if (IsValid(Tile->GetOccupyingPiece()) && !Tile->GetOccupyingPiece()->GetInstigator()->IsLocallyControlled())
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
		ConfirmationWidget = CreateWidget<UMatch_PyroActiveConfirmation>(GetWorld(), ActiveAbilityConfirmationClass, FName("Active Ability Confirmation Widget"));
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
		Cast<ABoardTile>(Target->GetCurrentTile())->Highlight->SetMaterial(0, Target->GetCurrentTile()->Highlight_Target);

		UE_LOG(LogTemp, Error, TEXT("Updated"));
	}
}

void APyromancer::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Zoom each player's camera into the fight
	 * Set attack animation to be the power-up animation.
	 * In the power-up animation, increase the piece's strength and make a strength buff pop-up
	 * When the animation ends, reset the attack animation variable.
	 * Change the pyromancer's attack animation.
	 * Start the attack sequence.
	 */

	BP_OnActiveAbility(Cast<AParentPiece>(Targets[0]));
}