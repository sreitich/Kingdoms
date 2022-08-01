// Copyright Samuel Reitich 2022.

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
	

/* Public variables. */
public:

    /* Info used when storing and loading this save. */
    UPROPERTY(VisibleAnywhere, Category="Save File Info")
    FString SaveSlotName;
 
    UPROPERTY(VisibleAnywhere, Category="Save File Info")
	int UserIndex;
    

	/* Array containing the IDs of each piece that this player has unlocked. */
	UPROPERTY(VisibleAnywhere, Category="Unlocked Pieces")
	TArray<FString> UnlockedPieces;
	
};
