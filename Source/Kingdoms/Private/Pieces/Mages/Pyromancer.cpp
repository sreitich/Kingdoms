// Copyright Samuel Reitich 2022.


#include "Pieces/Mages/Pyromancer.h"

#include "Board/BoardManager.h"
#include "Board/BoardTile.h"
#include "Framework/Match/Match_GameStateBase.h"

APyromancer::APyromancer()
{
}

void APyromancer::OnActiveAbility(AActor* Target)
{
	Super::OnActiveAbility(Target);

	UE_LOG(LogTemp, Error, TEXT("FIREBALL ACTIVATED!"));
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
		
		/* Valid tiles. */
		if
		(IsValid(Tile->GetOccupyingPiece()) &&
		(
			/* Forward 1 */
			(NewX == OldX && NewY == OldY + 1) ||
			/* Right 1 */
			(NewX == OldX + 1 && NewY == OldY) ||
			/* Backward 1 */
			(NewX == OldX && NewY == OldY - 1) ||
			/* Left 1 */
			(NewX == OldX - 1 && NewY == OldY) ||
				
			/* Forward 2 */
			(NewX == OldX && NewY == OldY + 2) ||
			/* Right 2 */
			(NewX == OldX + 2 && NewY == OldY) ||
			/* Backward 2 */
			(NewX == OldX && NewY == OldY - 2) ||
			/* Left 2 */
			(NewX == OldX - 2 && NewY == OldY) ||

			/* Forward 3 */
			(NewX == OldX && NewY == OldY + 3) ||
			/* Right 3 */
			(NewX == OldX + 3 && NewY == OldY) ||
			/* Backward 3 */
			(NewX == OldX && NewY == OldY - 3) ||
			/* Left 3 */
			(NewX == OldX - 3 && NewY == OldY)
		))
		{
			ValidTargets.Add(Cast<AActor>(Tile));
		}
	}

	return ValidTargets;
}
