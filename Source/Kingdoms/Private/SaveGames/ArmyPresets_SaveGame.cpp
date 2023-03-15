// Copyright Change Studios, LLC 2023.


#include "SaveGames/ArmyPresets_SaveGame.h"

UArmyPresets_SaveGame::UArmyPresets_SaveGame()
{
	/* These variables are used to store and load this save. */
	SaveSlotName = TEXT("ArmyPresetsSaveSlot");
	UserIndex = 0;

	/* Initialize five empty army presets. */
	ArmyPresets.Init(FArmyPresetStruct(), 5);
	
	/* 1 war mage, 1 paladin, 1 assassin, and 1 king. */
	ArmyPresets[0].ArmyName = TEXT("Starting Army");
	ArmyPresets[0].Pieces.Add(TEXT("S-03"));
	ArmyPresets[0].Pieces.Add(TEXT("P-01"));
	ArmyPresets[0].Pieces.Add(TEXT("R-01"));
	ArmyPresets[0].Pieces.Add(TEXT("C-01"));

	/* 3 academy recruits. */
	ArmyPresets[1].ArmyName = TEXT("Recruit test");
	ArmyPresets[1].Pieces.Add(TEXT("S-02"));
	ArmyPresets[1].Pieces.Add(TEXT("S-02"));
	ArmyPresets[1].Pieces.Add(TEXT("S-02"));

	/* 6 knights, 2 pyromancers, 2 guardians, 1 assassin, and 1 king. */
	ArmyPresets[2].ArmyName = TEXT("Game-Legal Army");
	ArmyPresets[2].Pieces.Add(TEXT("S-01"));
	ArmyPresets[2].Pieces.Add(TEXT("S-01"));
	ArmyPresets[2].Pieces.Add(TEXT("S-01"));
	ArmyPresets[2].Pieces.Add(TEXT("S-01"));
	ArmyPresets[2].Pieces.Add(TEXT("S-01"));
	ArmyPresets[2].Pieces.Add(TEXT("S-01"));
	ArmyPresets[2].Pieces.Add(TEXT("M-01"));
	ArmyPresets[2].Pieces.Add(TEXT("M-01"));
	ArmyPresets[2].Pieces.Add(TEXT("P-01"));
	ArmyPresets[2].Pieces.Add(TEXT("P-01"));
	// ArmyPresets[2].Pieces.Add(TEXT("R-01"));
	ArmyPresets[2].Pieces.Add(TEXT("C-01"));
}