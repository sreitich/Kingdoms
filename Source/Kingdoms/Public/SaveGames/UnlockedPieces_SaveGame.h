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
	static FString GetSaveSlotName();


/* Public variables. */	
public:

	/* Array containing the IDs of each piece that this player has unlocked. */
	UPROPERTY(BlueprintReadWrite, Category="Unlocked Pieces")
	TArray<FString> UnlockedPieces;


/* Private variables. */
private:

	/* Info used when storing and loading this save. */
	static const FString SaveSlotName;

};
