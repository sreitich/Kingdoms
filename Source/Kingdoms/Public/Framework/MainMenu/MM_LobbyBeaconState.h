// Copyright Change Studios, LLC 2023.

#pragma once

#include "Net/UnrealNetwork.h"

#include "CoreMinimal.h"
#include "LobbyBeaconState.h"
#include "MM_LobbyBeaconState.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class KINGDOMS_API AMM_LobbyBeaconState : public ALobbyBeaconState
{
	GENERATED_BODY()

/* Public functions. */
public:

	/* Default constructor. */
	AMM_LobbyBeaconState();
};
