// Copyright Samuel Reitich 2023.


#include "Pieces/Soldiers/Recruit.h"

#include "Board/BoardTile.h"
#include "Kismet/GameplayStatics.h"
#include "Pieces/PieceAIController.h"

ARecruit::ARecruit()
{
}

bool ARecruit::TileIsInMoveRange(ABoardTile* Tile)
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
			(NewX == OldX - 1 && NewY == OldY)
		)
		{
			return true;
		}
	}

	return false;
}

void ARecruit::BeginPlay()
{
	Super::BeginPlay();

}

void ARecruit::OnMoveToTileCompleted()
{
	Super::OnMoveToTileCompleted();

	/* Get this piece's row from the piece data. */
	static const FString ContextString(TEXT("Piece Data Struct"));
	const FPieceDataStruct* PieceData = PieceDataTable->FindRow<FPieceDataStruct>(PieceID, ContextString, true);

	if (PieceData)
	{
		/* Check every modifier to see if it's from this piece's passive ability. */
		for (FModifier Modifier : TemporaryModifiers)
		{
			if (Modifier.SourceAbilityName == PieceData->PassiveName)
			{
				/* Remove the recruit's passive ability modifier to make a new one. */
				// Server_RemoveModifier(Modifier, false);
				break;
			}
		}

		/* Get every recruit in the game. */
		TArray<AActor*> AllRecruitActors;
		UGameplayStatics::GetAllActorsOfClass(this, ARecruit::StaticClass(), AllRecruitActors);

		/* Increment AdjacentRecruitCount for every recruit adjacent to this one. */
		int AdjacentRecruitCount = 0;

		for (AActor* RecruitActor : AllRecruitActors)
		{
			if (const ARecruit* RecruitPtr = Cast<ARecruit>(RecruitActor))
			{
				if (TileIsAdjacent(RecruitPtr->GetCurrentTile()))
				{
					AdjacentRecruitCount++;
				}
			}
		}

		/* Create and add a new modifier based on how many recruits are adjacent to this one, if there were any. */
		if (AdjacentRecruitCount > 0)
		{
			const FModifier ModifierToAdd =
			{
				this,
				E_Friendly,
				PieceData->PieceName,
				PieceData->PassiveName,
				FModifier::Strength,
				AdjacentRecruitCount,
				-1
			};

			Server_AddModifier(ModifierToAdd, false);
		}
	}
}

bool ARecruit::TileIsAdjacent(ABoardTile* Tile)
{
	/* Save each coordinate to make the adjacency pattern easier to read. */
	const int XThis = GetCurrentTile()->Coordinates.X;
	const int YThis = GetCurrentTile()->Coordinates.Y;
	const int XOther = Tile->Coordinates.X;
	const int YOther = Tile->Coordinates.Y;

	/* Check if the given tile is laterally adjacent to this piece's tile. */
	return
	(
		/* 1 forward. */
		XThis == XOther && YThis == YOther + 1 ||
		/* 1 backward. */
		XThis == XOther && YThis == YOther - 1 ||
		/* 1 right. */
		XThis == XOther + 1 && YThis == YOther ||
		/* 1 left. */
		XThis == XOther - 1 && YThis == YOther
	);
}
