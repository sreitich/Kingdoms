// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_GameModeBase.h"

#include "Framework/MainMenu/MM_LobbyBeaconHostObject.h"
#include "UserInterface/MainMenu/MM_HUD.h"

#include "Kismet/GameplayStatics.h"
#include "OnlineBeaconHost.h"

AMM_GameModeBase::AMM_GameModeBase()
{
	HostObject = nullptr;
}

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

bool AMM_GameModeBase::CreateHostBeacon()
{
	if (AOnlineBeaconHost* Host = GetWorld()->SpawnActor<AOnlineBeaconHost>(AOnlineBeaconHost::StaticClass()))
	{
		if (Host->InitHost())
		{
			Host->PauseBeaconRequests(false);
			HostObject = GetWorld()->SpawnActor<AMM_LobbyBeaconHostObject>(AMM_LobbyBeaconHostObject::StaticClass());
			if (HostObject)
			{
				Host->RegisterHost(HostObject);
				return true;
			}
		}
	}

	return false;
}
