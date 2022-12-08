// Copyright Samuel Reitich 2023.


#include "Pieces/Commanders/ParentCommanderPiece.h"

#include "Framework/Match/Match_GameStateBase.h"
#include "Framework/Match/Match_PlayerState.h"
#include "Kismet/GameplayStatics.h"

void AParentCommanderPiece::OnDeath(AActor* Killer)
{
	/* Iterate through every player state to find the other player, whose commander didn't die. */
	if (AMatch_GameStateBase* GameStatePtr = Cast<AMatch_GameStateBase>(UGameplayStatics::GetGameState(this)))
	{
		for (APlayerState* PlayerStatePtr : GameStatePtr->PlayerArray)
		{
			if (PlayerStatePtr != GetInstigator()->GetPlayerState())
			{
				if (AMatch_PlayerState* Winner = Cast<AMatch_PlayerState>(PlayerStatePtr))
				{
					/* End the game with the other player as the winner and this commander's owning player as the loser. */
					GameStatePtr->Server_EndMatch(Winner);
				}
			}
		}
	}
}
