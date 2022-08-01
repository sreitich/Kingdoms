// Copyright Samuel Reitich 2022.


#include "SaveGames/UserSettings_SaveGame.h"

UUserSettings_SaveGame::UUserSettings_SaveGame()
{
	/* These variables are used to store and load this save. */
	SaveSlotName = TEXT("UserSettingsSaveSlot");
	UserIndex = 0;
}
