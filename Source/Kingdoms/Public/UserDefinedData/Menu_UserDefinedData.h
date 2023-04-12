// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"

#include "UserDefinedData/MapData.h"

#include "Menu_UserDefinedData.generated.h"

/* The menus available to the player. */
UENUM(BlueprintType)
enum EMenuScreen
{
	E_ArmyMenu			UMETA(DisplayName="Army Menu"),
	E_CollectionMenu	UMETA(DisplayName="Collection Menu"),
	E_LobbyMenu			UMETA(DisplayName="Lobby Menu"),
	E_MainMenu			UMETA(DisplayName="Main Menu"),
	E_MapSelection		UMETA(DisplayName="Map Selection Menu"),
	E_OptionsScreen		UMETA(DisplayName="Options Screen"),
	E_PlayMenu			UMETA(DisplayName="Play Menu"),
	E_StoreScreen		UMETA(DisplayName="Store Screen"),
	E_MenuDebug			UMETA(DisplayName="Menu Debug")
};

/* The settings and information for a custom game lobby. */
USTRUCT(BlueprintType)
struct FCustomLobbyData
{
	GENERATED_BODY()

	/* The currently selected map. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Lobby Data")
	UMapData* Map = nullptr;

	// Replace this with a custom struct of player information once we know what player info we want to display in the lobby.
	/* The displayed names of all connected players. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Custom Lobby Data")
	TArray<FString> PlayerNames = TArray<FString>();
};

USTRUCT(BlueprintType)
struct FCustomLobbyDataKeys
{
	GENERATED_BODY()

	/* The key used to retrieve a lobby's map. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Custom Lobby Data")
	FString Map_Key = "Map";
};