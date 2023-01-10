// Copyright Change Studios, LLC 2023.


#include "SaveGames/UnlockedPieces_SaveGame.h"

UUnlockedPieces_SaveGame::UUnlockedPieces_SaveGame()
{
	/* These variables are used to store and load this save. */
	SaveSlotName = TEXT("UnlockedPiecesSaveSlot");
	UserIndex = 0;
	
	/* Parent piece. */
	UnlockedPieces.Add(TEXT("0-00"));
	/* Play-testing mage. */
	UnlockedPieces.Add(TEXT("0-01"));
	/* Play-testing paladin. */
	UnlockedPieces.Add(TEXT("0-02"));
	
}
