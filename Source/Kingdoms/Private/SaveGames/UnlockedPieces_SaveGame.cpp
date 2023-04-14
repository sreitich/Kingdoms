// Copyright Change Studios, LLC 2023.


#include "SaveGames/UnlockedPieces_SaveGame.h"

const FString UUnlockedPieces_SaveGame::SaveSlotName = "UnlockedPiecesSaveSlot";

UUnlockedPieces_SaveGame::UUnlockedPieces_SaveGame()
{
	/* The knight, guardian, pyromancer, assassin, and king are the starting pieces. */
	UnlockedPieces = { "S-01", "P-01", "M-01", "R-01", "C-01" };
}

FString UUnlockedPieces_SaveGame::GetSaveSlotName()
{
	return SaveSlotName;
}