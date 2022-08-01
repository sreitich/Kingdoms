// Copyright Samuel Reitich 2022.


#include "SaveGames/ArmyPresets_SaveGame.h"

UArmyPresets_SaveGame::UArmyPresets_SaveGame()
{
	/* These variables are used to store and load this save. */
	SaveSlotName = TEXT("ArmyPresetsSaveSlot");
	UserIndex = 0;

	/* Initialize five empty army presets. */
	ArmyPresets.Init(FArmyPresetStruct(), 5);
	
	/* Default army preset 2 knights and 1 pyromancer. */
	ArmyPresets[0].ArmyName = TEXT("Starting Army");
	ArmyPresets[0].Pieces.Add(TEXT("S-01"));
	ArmyPresets[0].Pieces.Add(TEXT("S-01"));
	ArmyPresets[0].Pieces.Add(TEXT("M-01"));

	/* Default army preset 2 is comprised of 2 knights, 2 pyromancers, and 2 play-testing paladins. */
	ArmyPresets[1].ArmyName = TEXT("Double Trouble");
	ArmyPresets[1].Pieces.Add(TEXT("S-01"));
	ArmyPresets[1].Pieces.Add(TEXT("S-01"));
	ArmyPresets[1].Pieces.Add(TEXT("M-01"));
	ArmyPresets[1].Pieces.Add(TEXT("M-01"));
	ArmyPresets[1].Pieces.Add(TEXT("0-02"));
	ArmyPresets[1].Pieces.Add(TEXT("0-02"));

	/* Default army preset 3 is comprised of 1 knight and 4 pyromancers. */
	ArmyPresets[2].ArmyName = TEXT("Magic Attack");
	ArmyPresets[2].Pieces.Add(TEXT("S-01"));
	ArmyPresets[2].Pieces.Add(TEXT("M-01"));
	ArmyPresets[2].Pieces.Add(TEXT("M-01"));
	ArmyPresets[2].Pieces.Add(TEXT("M-01"));
	ArmyPresets[2].Pieces.Add(TEXT("M-01"));

	/* Default army preset 4 is comprised of a knight and 3 play-testing paladins. */
	ArmyPresets[3].ArmyName = TEXT("Royal Bodyguards");
	ArmyPresets[3].Pieces.Add(TEXT("S-01"));
	ArmyPresets[3].Pieces.Add(TEXT("0-02"));
	ArmyPresets[3].Pieces.Add(TEXT("0-02"));
	ArmyPresets[3].Pieces.Add(TEXT("0-02"));

}