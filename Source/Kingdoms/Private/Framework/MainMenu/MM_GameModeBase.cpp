// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_GameModeBase.h"

void AMM_GameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	/* If there are now enough players, start the game by traveling to the game map as a listen server. */
	if (GetNumPlayers() == 2)
	{
		GetWorld()->ServerTravel("/Game/Maps/Playtesting/Testing_TileRework?listen");
	}
}
