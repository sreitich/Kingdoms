// Copyright Samuel Reitich 2023.


#include "Pieces/Commanders/King.h"

#include "Blueprint/UserWidget.h"
#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
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

	/* Create the active ability confirmation widget if it hasn't been created yet. */
	if (!ActiveAbilityConfirmationWidget)
	{
		/* Create an instance of this piece's active ability confirmation widget. */
		ActiveAbilityConfirmationWidget = CreateWidget<UKing_ActiveAbilityConfirmation>(GetWorld(), ActiveAbilityConfirmationClass, FName("King Active Ability Confirmation Widget"));

		/* Update the active ability confirmation widget and add it to the viewport if it was created successfully. */
		if (ActiveAbilityConfirmationWidget)
		{
			/* Get all friendly pieces, which will all be the target of this piece's active ability. */
			const TArray<AActor*> Targets = GetValidActiveAbilityTargets();

			/* Update the widget's information. */
			Cast<UKing_ActiveAbilityConfirmation>(ActiveAbilityConfirmationWidget)->Widget_UpdateActiveConfirmation(this, Targets);
			ActiveAbilityConfirmationWidget->AddToViewport(0);
		}
	}
}

TArray<AActor*> AKing::GetValidActiveAbilityTargets()
{
	/* This ability has infinite range and all friendly pieces are valid targets. */
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

TArray<ABoardTile*> AKing::GetActiveAbilityRange()
{
	/* Get and return every tile since this ability has infinite range. */
	TArray<AActor*> AllTileActors;
	TArray<ABoardTile*> AllTiles;
	UGameplayStatics::GetAllActorsOfClass(this, ABoardTile::StaticClass(), OUT AllTileActors);

	/* Cast every tile actor to a board tile to return the correct type. */
	for (AActor* Tile : AllTileActors)
		if (ABoardTile* CastTile = Cast<ABoardTile>(Tile))
			AllTiles.Add(CastTile);
	
	return AllTiles;
}

void AKing::OnActiveAbility(TArray<AActor*> Targets)
{
	/* Use the player's active ability action, trigger its cooldown, and decrement its uses. */
	Super::OnActiveAbility(Targets);

	/* Triggers an animation which provides every friendly piece with a +2A modifier. */
	BP_OnActiveAbility();
}
