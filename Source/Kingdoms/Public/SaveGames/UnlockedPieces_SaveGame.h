// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UnlockedPieces_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UUnlockedPieces_SaveGame : public USaveGame
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	UUnlockedPieces_SaveGame();

	/* Getter for this save game's slot name. */
	UFUNCTION(BlueprintPure, Category="Unlocked Pieces")
	static FString GetUnlockedPiecesSaveSlotName();


	/* Getter for UnlockedPieces array. */
	UFUNCTION(BlueprintPure, Category="Unlocked Pieces")
	TArray<FString> GetUnlockedPieces() const { return UnlockedPieces; }

	/* Adds the given ID to the array of unlocked pieces, if it isn't already there. */
	UFUNCTION(BlueprintCallable, Category="Unlocked Pieces")
	void UnlockPiece(FString PieceIDToUnlock);

	/* Removes the given ID from the array of unlocked pieces, if it's there. */
	UFUNCTION(BlueprintCallable, Category="Unlocked Pieces")
	void LockPiece(FString PieceIDToLock);


/* Private variables. */
private:

	/* Array containing the IDs of each piece that this player has unlocked. */
	UPROPERTY()
	TArray<FString> UnlockedPieces;

	/* Info used when storing and loading this save. */
	static const FString SaveSlotName;

};
