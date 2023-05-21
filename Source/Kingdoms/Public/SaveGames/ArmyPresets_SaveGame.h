// Copyright Change Studios, LLC 2023.

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

	/* Getter for this save game's slot name. */
	UFUNCTION(BlueprintPure, Category="Army Presets")
	static FString GetArmyPresetsSaveSlotName();

	/* Getter for ArmyPresets array. */
	UFUNCTION(BlueprintPure, Category="Army Presets")
	TArray<FArmyPresetStruct> GetArmyPresets() const { return ArmyPresets; }

	/* Sets the given saved preset to the given army preset. */
	UFUNCTION(BlueprintCallable, Category="Army Presets")
	void SaveArmyPreset(uint8 SlotIndex, FArmyPresetStruct NewArmy);


/* Private variables. */
private:

	/* Player-created army presets, containing a list of piece IDs for the army's composition. */
	UPROPERTY(VisibleAnywhere, Category="Army Presets")
	TArray<FArmyPresetStruct> ArmyPresets;

	/* Info used when storing and loading this save. */
	static const FString SaveSlotName;
};
