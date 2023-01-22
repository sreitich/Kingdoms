// Copyright Change Studios, LLC 2023.

#pragma once

#include "CoreMinimal.h"

// #include "Menu_UserDefinedData.generated.h"

/* The menus available to the player. */
UENUM(BlueprintType)
enum EMenuScreen
{
	E_ArmyMenu		UMETA(DisplayName="Army Menu"),
	E_MainMenu		UMETA(DisplayName="Main Menu"),
	E_OptionsScreen	UMETA(DisplayName="Options Screen"),
	E_PlayMenu		UMETA(DisplayName="Play Menu"),
	E_StoreScreen	UMETA(DisplayName="Store Screen")
};
