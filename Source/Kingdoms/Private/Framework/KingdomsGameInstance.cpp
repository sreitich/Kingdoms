// Copyright Change Studios, LLC 2023.


#include "Framework/KingdomsGameInstance.h"

#include "SaveGames/UnlockedPieces_SaveGame.h"
#include "SaveGames/ArmyPresets_SaveGame.h"

#include "Kismet/GameplayStatics.h"

void UKingdomsGameInstance::Init()
{
	Super::Init();

	/* If a save slot exists for unlocked pieces... */
	if (UnlockedPieces_SaveGame && UGameplayStatics::DoesSaveGameExist(UnlockedPieces_SaveGame->SaveSlotName, UnlockedPieces_SaveGame->UserIndex))
	{
		/* Load the save slot into the save game object. */
		UnlockedPieces_SaveGame = Cast<UUnlockedPieces_SaveGame>(UGameplayStatics::LoadGameFromSlot(UnlockedPieces_SaveGame->SaveSlotName, UnlockedPieces_SaveGame->UserIndex));
	}
	/* If a save slot hasn't yet been created for unlocked pieces... */
	else
	{
		/* Create a new save slot and load it into the save game object. */
		UnlockedPieces_SaveGame = Cast<UUnlockedPieces_SaveGame>(UGameplayStatics::CreateSaveGameObject(UUnlockedPieces_SaveGame::StaticClass()));
	}
	
	/* If a save slot exists for army presets... */
	if (ArmyPresets_SaveGame && UGameplayStatics::DoesSaveGameExist(ArmyPresets_SaveGame->SaveSlotName, ArmyPresets_SaveGame->UserIndex))
	{
		/* Load the save slot into the save game object. */
		ArmyPresets_SaveGame = Cast<UArmyPresets_SaveGame>(UGameplayStatics::LoadGameFromSlot(ArmyPresets_SaveGame->SaveSlotName, ArmyPresets_SaveGame->UserIndex));
	}
	/* If a save slot hasn't yet been created for army presets... */
	else
	{
		/* Create a new save slot and load it into the save game object. */
		ArmyPresets_SaveGame = Cast<UArmyPresets_SaveGame>(UGameplayStatics::CreateSaveGameObject(UArmyPresets_SaveGame::StaticClass()));
	}
}
