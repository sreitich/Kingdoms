// Copyright Samuel Reitich 2022.


#include "SaveGames/ArmyPresets_SaveGame.h"

UArmyPresets_SaveGame::UArmyPresets_SaveGame()
{
	/* These variables are used to store and load this save. */
	SaveSlotName = TEXT("ArmyPresetsSaveSlot");
	UserIndex = 0;

	/* Initialize five empty army presets. */
	ArmyPresets.Init(FArmyPresetStruct(), 5);
	
	/* 1 knight, 1 academy recruit, and 1 pyromancer. */
	ArmyPresets[0].ArmyName = TEXT("Starting Army");
	ArmyPresets[0].Pieces.Add(TEXT("S-01"));
	ArmyPresets[0].Pieces.Add(TEXT("S-02"));
	ArmyPresets[0].Pieces.Add(TEXT("M-01"));

	/* 3 academy recruits. */
	ArmyPresets[1].ArmyName = TEXT("Recruit test");
	ArmyPresets[1].Pieces.Add(TEXT("S-02"));
	ArmyPresets[1].Pieces.Add(TEXT("S-02"));
	ArmyPresets[1].Pieces.Add(TEXT("S-02"));
}