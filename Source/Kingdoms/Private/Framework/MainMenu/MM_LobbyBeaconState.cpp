// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_LobbyBeaconState.h"

#include "Framework/MainMenu/MM_LobbyBeaconPlayerState.h"

AMM_LobbyBeaconState::AMM_LobbyBeaconState()
{
	/* Set which player state class to use for players connected to this lobby. */
	LobbyBeaconPlayerStateClass = AMM_LobbyBeaconPlayerState::StaticClass();
}
