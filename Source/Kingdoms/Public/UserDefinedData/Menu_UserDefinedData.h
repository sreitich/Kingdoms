// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"

// #include "Menu_UserDefinedData.generated.h"

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
