// Copyright Samuel Reitich 2022.

#pragma once

#include "UserDefinedData/SaveGameData_UserDefinedData.h"

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ArmyPresets_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_API UArmyPresets_SaveGame : public USaveGame
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	UArmyPresets_SaveGame();


/* Public variables. */
public:

    /* Info used when storing and loading this save. */
    UPROPERTY(VisibleAnywhere, Category="Save File Info")
    FString SaveSlotName;
 
    UPROPERTY(VisibleAnywhere, Category="Save File Info")
    int UserIndex;

	/* Player-created army presets, containing a list of piece IDs for the army's composition. */
	UPROPERTY(VisibleAnywhere, Category="Army Presets")
	TArray<FArmyPresetStruct> ArmyPresets;

};
