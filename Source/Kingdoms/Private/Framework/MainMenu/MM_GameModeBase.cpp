// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_GameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "UserInterface/MainMenu/MM_HUD.h"

void AMM_GameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	/* If there are now enough players, start the game by traveling to the game map as a listen server. */
	if (GetNumPlayers() == 2)
	{
		/* Play the "found opponent" pop-up and disable the "cancel" button. */
		if (AMM_HUD* HUDPtr = Cast<AMM_HUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD()))
		{
			HUDPtr->HUD_FoundOpponent();
		}

		GetWorld()->ServerTravel("/Game/Maps/Playtesting/Testing_TileRework?listen");
	}
}
