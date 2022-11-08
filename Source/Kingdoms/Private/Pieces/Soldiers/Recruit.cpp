// Copyright Samuel Reitich 2023.


#include "Pieces/Soldiers/Recruit.h"

#include "Board/BoardTile.h"
#include "Kismet/GameplayStatics.h"
#include "Pieces/PieceAIController.h"

ARecruit::ARecruit()
{
}

void ARecruit::OnGameStart()
{
	/* Update this piece's passive ability modifier at the start of the game, in case it was placed adjacent to any
	 * other recruits. */
	OnMoveToTileCompleted();
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
	/* Remove this recruit from the adjacent recruit array of any recruits that are no longer adjacent to this one. Then
	 * update the passive modifiers of those recruits with their new amounts of adjacent recruits. */	
	TArray<ARecruit*> NoLongerAdjacentRecruits;

	for (ARecruit* RecruitPtr : AdjacentRecruits)
	{
		if (!TileIsAdjacent(RecruitPtr->GetCurrentTile()))
		{
			RecruitPtr->AdjacentRecruits.Remove(this);
			RecruitPtr->UpdatePassiveModifier(false);

			/* Save the recruits that are no longer adjacent to this recruit. */
			NoLongerAdjacentRecruits.Add(RecruitPtr);
		}
	}

	/* Remove the recruits that are no longer adjacent from this recruit's array of adjacent recruits. */
	for (ARecruit* RecruitPtr : NoLongerAdjacentRecruits)
	{
		AdjacentRecruits.Remove(RecruitPtr);
	}


	/* Get every recruit in the game. */
	TArray<AActor*> AllRecruitActors;
	TArray<ARecruit*> NewlyAdjacentRecruits;
	UGameplayStatics::GetAllActorsOfClass(this, ARecruit::StaticClass(), AllRecruitActors);

	/* Add any recruits that are now adjacent to the array of adjacent recruits. */
	for (AActor* RecruitActor : AllRecruitActors)
	{
		if (ARecruit* RecruitPtr = Cast<ARecruit>(RecruitActor))
		{
			if (TileIsAdjacent(RecruitPtr->GetCurrentTile()) && !AdjacentRecruits.Contains(RecruitPtr) && RecruitPtr->GetAlignment() == E_Friendly)
			{
				AdjacentRecruits.Add(RecruitPtr);
				NewlyAdjacentRecruits.Add(RecruitPtr);
			}
		}
	}


	/* Add this recruit to every newly-adjacent recruit's array of adjacent recruits, and update their passive ability
	 * modifiers with their updated array. Trigger a modifier pop-up for each one. */
	for (ARecruit* NewlyAdjacentRecruit : NewlyAdjacentRecruits)
	{
		NewlyAdjacentRecruit->AdjacentRecruits.Add(this);
		NewlyAdjacentRecruit->UpdatePassiveModifier(true);
	}

	/* Update this recruit's passive modifier with its update array of adjacent recruits. Trigger a modifier pop-up. */
	UpdatePassiveModifier(true);
}

void ARecruit::UpdatePassiveModifier(bool bTriggerPopUp)
{
	/* Get this piece's row from the piece data. */
	static const FString ContextString(TEXT("Piece Data Struct"));
	const FPieceDataStruct* PieceData = PieceDataTable->FindRow<FPieceDataStruct>(PieceID, ContextString, true);

	if (PieceData)
	{
		/* Find and remove the modifier caused by the recruit's passive ability, if there is one, either to update it or
		 * to remove it indefinitely. Don't trigger a pop-up when a recruit loses its passive modifier. */
		for (FModifier Modifier : TemporaryModifiers)
		{
			if (Modifier.SourceAbilityName == PieceData->PassiveName)
			{
				Server_RemoveModifier(Modifier, false);
				break;
			}
		}

		/* Add a new passive ability modifier depending on how many recruits are now adjacent to this one. */
		if (AdjacentRecruits.Num() > 0)
		{
			const FModifier ModifierToAdd =
			{
				this,
				E_Friendly,
				PieceData->PieceName,
				PieceData->PassiveName,
				FModifier::Strength,
				AdjacentRecruits.Num(),
				-1
			};

			Server_AddModifier(ModifierToAdd, bTriggerPopUp, bTriggerPopUp);
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
