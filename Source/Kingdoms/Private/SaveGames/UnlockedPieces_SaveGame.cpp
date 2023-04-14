// Copyright Change Studios, LLC 2023.


#include "SaveGames/UnlockedPieces_SaveGame.h"

#include "Kismet/GameplayStatics.h"

const FString UUnlockedPieces_SaveGame::SaveSlotName = "UnlockedPiecesSaveSlot";

UUnlockedPieces_SaveGame::UUnlockedPieces_SaveGame()
{
	/* The knight, guardian, pyromancer, assassin, and king are the starting pieces. */
	UnlockedPieces = { "S-01", "P-01", "M-01", "R-01", "C-01" };
}

FString UUnlockedPieces_SaveGame::GetUnlockedPiecesSaveSlotName()
{
	return SaveSlotName;
}

void UUnlockedPieces_SaveGame::UnlockPiece(FString PieceIDToUnlock)
{
	UnlockedPieces.AddUnique(PieceIDToUnlock);

	UGameplayStatics::SaveGameToSlot(this, SaveSlotName, 0);
}

void UUnlockedPieces_SaveGame::LockPiece(FString PieceIDToLock)
{
	UnlockedPieces.Remove(PieceIDToLock);

	UGameplayStatics::SaveGameToSlot(this, SaveSlotName, 0);
}
