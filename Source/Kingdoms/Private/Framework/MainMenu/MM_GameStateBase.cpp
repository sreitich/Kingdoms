// Copyright Change Studios, LLC 2023.


#include "Framework/MainMenu/MM_GameStateBase.h"

void AMM_GameStateBase::SetCustomGameSettings(FCustomLobbyData NewCustomGameSettings)
{
	/* Update the local player's custom game settings. */
	CustomGameSettings = NewCustomGameSettings;
}
