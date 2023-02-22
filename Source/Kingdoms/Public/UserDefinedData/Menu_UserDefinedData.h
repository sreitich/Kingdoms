// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"

#include "UserDefinedData/MapData.h"

#include "Menu_UserDefinedData.generated.h"

/* The menus available to the player. */
UENUM(BlueprintType)
enum EMenuScreen
{
	E_ArmyMenu		UMETA(DisplayName="Army Menu"),
	E_LobbyMenu		UMETA(DisplayName="Lobby Menu"),
	E_MainMenu		UMETA(DisplayName="Main Menu"),
	E_MapSelection	UMETA(DisplayName="Map Selection Menu"),
	E_OptionsScreen	UMETA(DisplayName="Options Screen"),
	E_PlayMenu		UMETA(DisplayName="Play Menu"),
	E_StoreScreen	UMETA(DisplayName="Store Screen"),
	E_MenuDebug		UMETA(DisplayName="Menu Debug")
};

/* The settings and information for a custom game lobby. */
USTRUCT(BlueprintType)
struct FCustomLobbyInformation
{
	GENERATED_BODY()

	/* The currently selected map. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Lobby Info")
	UMapData* Map = nullptr;

	// Replace this with a custom struct of player information once we know what player info we want to display in the lobby.
	/* The displayed names of all connected players. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Custom Lobby Info")
	TArray<FString> PlayerNames = TArray<FString>();
};