// Copyright Samuel Reitich 2023.


#include "Pieces/Soldiers/Recruit.h"

#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Components/PieceNetworkingComponent.h"
#include "Framework/Match/Match_GameStateBase.h"
#include "Framework/Match/Match_PlayerPawn.h"
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

TArray<AActor*> ARecruit::GetValidPassiveAbilityTargets()
{
	/* Get every recruit in the game. */
	TArray<AActor*> AllRecruitActors;
	TArray<AActor*> ValidPassiveAbilityTargets;
	UGameplayStatics::GetAllActorsOfClass(this, ARecruit::StaticClass(), AllRecruitActors);

	/* Add any adjacent recruits to the array of valid passive ability targets. */
	for (AActor* RecruitActor : AllRecruitActors)
	{
		if (ARecruit* RecruitPtr = Cast<ARecruit>(RecruitActor))
		{
			/* If the iterated recruit is laterally adjacent to this recruit and is friendly, it's a valid passive
			 * ability target. */
			if (GetCurrentTile()->IsAdjacentTo(false, RecruitPtr->GetCurrentTile()) && RecruitPtr->GetInstigator() == GetInstigator())
			{
				ValidPassiveAbilityTargets.Add(RecruitPtr);
			}
		}
	}

	return ValidPassiveAbilityTargets;
}

TArray<ABoardTile*> ARecruit::GetPassiveAbilityRange()
{
	TArray<ABoardTile*> TilesInRange;

	/* Get the board manager's array of every tile on the board. */
	for (ABoardTile* Tile : GetWorld()->GetGameState<AMatch_GameStateBase>()->BoardManager->AllTiles)
	{
		/* If the iterated tile is laterally adjacent to this piece's current tile, it's within this piece's passive
		 * ability's range. */
		if (GetCurrentTile()->IsAdjacentTo(false, Tile))
		{
			TilesInRange.Add(Tile);
		}
	}

	return TilesInRange;
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
		if (!GetCurrentTile()->IsAdjacentTo(false, RecruitPtr->GetCurrentTile()))
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
			/* A recruit can trigger the passive ability if it is laterally adjacent, became adjacent with the most
			 * recent move, and has the same instigator (i.e. is friendly). The IsFriendly() function doesn't work on
			 * clients here for some reason. */
			if (GetCurrentTile()->IsAdjacentTo(false, RecruitPtr->GetCurrentTile()) && !AdjacentRecruits.Contains(RecruitPtr) && RecruitPtr->GetInstigator() == GetInstigator())
			{
				AdjacentRecruits.Add(RecruitPtr);
				NewlyAdjacentRecruits.Add(RecruitPtr);
			}
		}
	}


	/* Don't change other recruits' modifiers if this is the game just started. Every recruit manages its modifiers initially. */
	if (bPlaced)
	{
		/* Add this recruit to every newly-adjacent recruit's array of adjacent recruits, and update their passive ability
		 * modifiers with their updated array. Trigger a modifier pop-up for each recruit.. */
		for (ARecruit* NewlyAdjacentRecruit : NewlyAdjacentRecruits)
		{
			NewlyAdjacentRecruit->AdjacentRecruits.Add(this);
			NewlyAdjacentRecruit->UpdatePassiveModifier(true);
		}
	}

	/* Update this recruit's passive modifier with its update array of adjacent recruits. Trigger a modifier pop-up. */
	UpdatePassiveModifier(true);

	/* After the game starts, always trigger other recruits' pop-ups. */
	bPlaced = true;
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
				Cast<AMatch_PlayerPawn>(GetInstigator())->GetPieceNetworkingComponent()->Server_RemoveModifier(this, Modifier, false, false);
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
				AdjacentRecruits.Num() * PassiveStrengthBuffPerPiece,
				AdjacentRecruits.Num() * PassiveArmorBuffPerPiece,
				true,
				-1,
				false,
				false
			};

			Cast<AMatch_PlayerPawn>(GetInstigator())->GetPieceNetworkingComponent()->Server_AddModifier(this, ModifierToAdd, bTriggerPopUp);
		}
	}
}