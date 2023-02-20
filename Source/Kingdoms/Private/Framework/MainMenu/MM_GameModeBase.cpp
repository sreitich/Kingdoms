// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_GameModeBase.h"

#include "Framework/MainMenu/MM_LobbyBeaconHostObject.h"
#include "UserInterface/MainMenu/MM_HUD.h"

#include "Kismet/GameplayStatics.h"
#include "OnlineBeaconHost.h"

AMM_GameModeBase::AMM_GameModeBase()
{
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
	/* Spawn an online beacon host to connect incoming beacon client connections to the beacon host object. */
	BeaconHost = GetWorld()->SpawnActor<AOnlineBeaconHost>(AOnlineBeaconHost::StaticClass());
	if (IsValid(BeaconHost))
	{
		/* Initialize our beacon host. */
		if (BeaconHost->InitHost())
		{
			/* Allow remote client machines to send requests to connect to this beacon. */
			BeaconHost->PauseBeaconRequests(false);

			/* Spawn a beacon host object to manage connected client beacons. */
			BeaconHostObject = GetWorld()->SpawnActor<AMM_LobbyBeaconHostObject>(AMM_LobbyBeaconHostObject::StaticClass());
			if (IsValid(BeaconHostObject))
			{
				/* Register the host object if it was successfully spawned. */
				BeaconHost->RegisterHost(BeaconHostObject);
				/* Set up the lobby's state. */
				BeaconHostObject->SetupLobbyState(2);
				UE_LOG(LogTemp, Error, TEXT("Gamemode: Registered host"));
				return true;
			}
		}
	}

	return false;
}
