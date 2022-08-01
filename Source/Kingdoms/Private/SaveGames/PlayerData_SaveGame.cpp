// Copyright Samuel Reitich 2022.


#include "SaveGames/PlayerData_SaveGame.h"

UPlayerData_SaveGame::UPlayerData_SaveGame()
{
	/* These variables are used to store and load this save. */
	SaveSlotName = TEXT("PlayerDataSaveSlot");
	UserIndex = 0;
}
