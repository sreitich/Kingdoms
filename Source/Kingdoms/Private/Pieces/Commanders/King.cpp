// Copyright Samuel Reitich 2023.


#include "Pieces/Commanders/King.h"

#include "Blueprint/UserWidget.h"
#include "Board/BoardTile.h"
#include "Components/PieceNetworkingComponent.h"
#include "Framework/Match/Match_PlayerPawn.h"
#include "Pieces/ParentPiece.h"
#include "UserInterface/Match/ActiveAbilityConfirmations/Commanders/King_ActiveAbilityConfirmation.h"

#include "Kismet/GameplayStatics.h"

AKing::AKing()
{
}

bool AKing::TileIsInMoveRange(ABoardTile* Tile)
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
		
			/* Forward 1, right 1 */
			(NewX == OldX + 1 && NewY == OldY + 1) ||
			/* Forward 1, left 1 */
			(NewX == OldX - 1 && NewY == OldY + 1) ||
			/* Backward 1, right 1 */
			(NewX == OldX + 1 && NewY == OldY - 1) ||
			/* Backward 1, left 1 */
			(NewX == OldX - 1 && NewY == OldY - 1)
		)
		{
			return true;
		}
	}
	
	return false;
}

TArray<AActor*> AKing::GetActiveAbilityRange()
{
	TArray<AActor*> AllPiecesActors;
	TArray<AActor*> AllFriendlyPieces;

	/* Get every piece. */
	UGameplayStatics::GetAllActorsOfClass(this, AParentPiece::StaticClass(), OUT AllPiecesActors);

	/* Add every friendly piece to an array of all friendly pieces. */
	for (AActor* PieceActor : AllPiecesActors)
	{
		if (AParentPiece* PieceCast = Cast<AParentPiece>(PieceActor))
		{
			/* If the target piece is friendly to this piece. */
			if (PieceCast->GetInstigator() == GetInstigator())
				AllFriendlyPieces.Add(PieceCast);
		}
	}

	return AllFriendlyPieces;
}

TArray<AActor*> AKing::GetValidActiveAbilityTargets()
{
	/* This ability has infinite range and all friendly pieces are valid targets. */
	return GetActiveAbilityRange();
}

void AKing::OnActiveClicked()
{
	/* Highlight all friendly pieces and their tiles. */
	for (AActor* Target : GetValidActiveAbilityTargets())
	{
		if (AParentPiece* Piece = Cast<AParentPiece>(Target))
		{
			/* Highlight each piece as "selected." */
			Piece->FlashHighlight(Piece->FriendlyFresnelColor, Piece->SelectedFresnelStrength, 1.0f, 0.0f, true);

			/* Highlight each tile. */
			ABoardTile* Tile = Piece->GetCurrentTile();
			Tile->UpdateEmissiveHighlight(true, Tile->DefaultHighlightPlayRate, Tile->Highlight_Friendly);
		}
	}

	/* If the confirmation widget hasn't been created yet, create it. */
	if (!ActiveAbilityConfirmationWidget)
	{
		/* Create an ability confirmation widget. */
		ActiveAbilityConfirmationWidget = CreateWidget<UKing_ActiveAbilityConfirmation>(GetWorld(), ActiveAbilityConfirmationClass, FName("Active Ability Confirmation Widget"));
		ActiveAbilityConfirmationWidget->AddToViewport(0);
	}

	/* Get all friendly pieces. */
	const TArray<AActor*> Targets = GetValidActiveAbilityTargets();
	
	/* Update the widget's information. */
	Cast<UKing_ActiveAbilityConfirmation>(ActiveAbilityConfirmationWidget)->UpdateActionConfirmationInfo(this, Targets);
}

void AKing::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Use the player's active ability action, trigger its cooldown, and decrement its uses. */
	Super::OnActiveAbility(Targets);

	/* Triggers an animation which provides every friendly piece with a +2A modifier. */
	BP_OnActiveAbility();
}
